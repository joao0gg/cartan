// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include <Eigen/Dense>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "core/SimplicialComplex.h"
#include "discretization/HodgeStar.h"
#include "discretization/Laplacian.h"
#include "geometry/Metric.h"

namespace {

using cartan::core::Index;
using cartan::core::SimplicialComplex;
using cartan::core::SparseMatrix;
using cartan::discretization::DualArea;
using cartan::geometry::Metric;

namespace discretization = cartan::discretization;

int failures = 0;

void expect(bool condition, const char *what) {
  if (!condition) {
    std::fprintf(stderr, "FAILED: %s\n", what);
    ++failures;
  }
}

template <class Function> bool throws(Function function) {
  try {
    function();
  }
  catch (const std::exception &) {
    return true;
  }

  return false;
}

bool near(double a, double b, double tolerance = 1e-12) {
  return std::abs(a - b) <= tolerance * std::max(1.0, std::abs(b));
}

struct Surface {
  std::vector<glm::dvec3> positions;
  std::vector<Index> triangles;
};

Surface icosphere(int subdivisions) {
  const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
  Surface surface;
  surface.positions = {{-1, phi, 0}, {1, phi, 0}, {-1, -phi, 0}, {1, -phi, 0},
                       {0, -1, phi}, {0, 1, phi}, {0, -1, -phi}, {0, 1, -phi},
                       {phi, 0, -1}, {phi, 0, 1}, {-phi, 0, -1}, {-phi, 0, 1}};
  surface.triangles = {0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
                       4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
                       6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1};

  for (int level = 0; level < subdivisions; ++level) {
    std::map<std::pair<Index, Index>, Index> midpoints;
    std::vector<Index> refined;

    const auto midpoint = [&](Index a, Index b) {
      const auto key = std::minmax(a, b);
      const auto [found,
                  inserted] = midpoints.emplace(key, static_cast<Index>(surface.positions.size()));

      if (inserted) {
        surface.positions.push_back(0.5 * (surface.positions[a] + surface.positions[b]));
      }

      return found->second;
    };

    for (std::size_t t = 0; t < surface.triangles.size(); t += 3) {
      const Index a = surface.triangles[t];
      const Index b = surface.triangles[t + 1];
      const Index c = surface.triangles[t + 2];
      const Index ab = midpoint(a, b);
      const Index bc = midpoint(b, c);
      const Index ca = midpoint(c, a);
      refined.insert(refined.end(), {a, ab, ca, b, bc, ab, c, ca, bc, ab, bc, ca});
    }

    surface.triangles = std::move(refined);
  }

  for (auto &position : surface.positions) {
    position = glm::normalize(position);
  }

  return surface;
}

SimplicialComplex complexOf(const Surface &surface) {
  return SimplicialComplex::fromTopSimplices(2, static_cast<Index>(surface.positions.size()),
                                             surface.triangles);
}

double sphereLaplacianError(int subdivisions, DualArea dualArea) {
  const auto surface = icosphere(subdivisions);
  const auto complex = complexOf(surface);
  const auto metric = Metric::fromPositions(complex, surface.positions);
  const SparseMatrix star0 = discretization::hodgeStar0(metric, dualArea);
  const SparseMatrix L = discretization::laplacian(complex, discretization::hodgeStar1(metric));
  Eigen::VectorXd z(complex.count(0));

  for (Index v = 0; v < complex.count(0); ++v) {
    z[v] = surface.positions[v].z;
  }

  const Eigen::VectorXd applied = L * z;
  double error = 0.0;

  for (Index v = 0; v < complex.count(0); ++v) {
    error = std::max(error, std::abs(applied[v] / star0.coeff(v, v) - 2.0 * z[v]));
  }

  return error;
}

double total(const SparseMatrix &diagonal) {
  return Eigen::VectorXd(diagonal.diagonal()).sum();
}

void testLaplacianIdentities() {
  const auto surface = icosphere(2);
  const auto complex = complexOf(surface);
  const auto metric = Metric::fromPositions(complex, surface.positions);
  const SparseMatrix L = discretization::laplacian(complex, discretization::hodgeStar1(metric));
  const Eigen::VectorXd ones = Eigen::VectorXd::Ones(complex.count(0));
  bool signs = true;

  for (int column = 0; column < L.outerSize(); ++column) {
    for (SparseMatrix::InnerIterator entry(L, column); entry; ++entry) {
      signs = signs && (entry.row() == entry.col() ? entry.value() > 0.0 : entry.value() <= 0.0);
    }
  }

  expect((L * ones).norm() < 1e-12, "Laplacian: constants are in the kernel");
  expect(SparseMatrix(L - SparseMatrix(L.transpose())).norm() < 1e-14, "Laplacian: symmetric");
  expect(signs, "Laplacian: positive diagonal, non-positive off-diagonal on a Delaunay mesh");
  expect(L.nonZeros() == complex.count(0) + 2 * complex.count(1),
         "Laplacian: one off-diagonal pair per edge");
}

void testCotanFormula() {
  const auto complex = SimplicialComplex::fromTopSimplices(2, 4,
                                                           std::vector<Index>{0, 1, 2, 0, 2, 3});
  const std::vector<glm::dvec3> positions{{0, 0, 0},
                                          {1.3, -0.2, 0.1},
                                          {1.1, 0.9, -0.3},
                                          {-0.2, 1.0, 0.2}};
  const auto metric = Metric::fromPositions(complex, positions);
  const SparseMatrix L = discretization::laplacian(complex, discretization::hodgeStar1(metric));

  expect(near(L.coeff(0, 2), -0.5 * (metric.cotangent(0, 1) + metric.cotangent(1, 2))),
         "Laplacian: interior edge weight is minus half the sum of opposite cotangents");
  expect(near(L.coeff(0, 1), -0.5 * metric.cotangent(0, 2)),
         "Laplacian: boundary edge weight is minus half the opposite cotangent");
}

void testDualAreas() {
  const auto surface = icosphere(2);
  const auto complex = complexOf(surface);
  const auto metric = Metric::fromPositions(complex, surface.positions);
  double area = 0.0;

  for (const double triangle : metric.volumes(2)) {
    area += triangle;
  }

  const SparseMatrix barycentric = discretization::hodgeStar0(metric, DualArea::Barycentric);
  const SparseMatrix circumcentric = discretization::hodgeStar0(metric, DualArea::Circumcentric);

  expect(near(total(barycentric), area), "barycentric dual areas sum to the surface area");
  expect(near(total(circumcentric), area), "circumcentric dual areas sum to the surface area");
  expect(Eigen::VectorXd(barycentric.diagonal()).minCoeff() > 0.0,
         "barycentric dual areas are positive");
}

void testObtuseTriangle() {
  const auto complex = SimplicialComplex::fromTopSimplices(2, 3, std::vector<Index>{0, 1, 2});
  const std::vector<glm::dvec3> positions{{0, 0, 0}, {2, 0, 0}, {1, 0.2, 0}};
  const auto metric = Metric::fromPositions(complex, positions);
  const SparseMatrix circumcentric = discretization::hodgeStar0(metric, DualArea::Circumcentric);

  expect(near(total(circumcentric), metric.volume(2, 0)),
         "obtuse: dual areas still sum to the area");
  expect(circumcentric.coeff(0, 0) < 0.0, "obtuse: circumcentric dual area can be negative");
}

void testSphereConvergence() {
  const double coarse = sphereLaplacianError(4, DualArea::Circumcentric);
  const double fine = sphereLaplacianError(5, DualArea::Circumcentric);

  expect(fine < 1e-3, "sphere: circumcentric Laplacian of z approximates 2z");
  expect(coarse / fine > 3.5, "sphere: second-order convergence");
}

void testGenericStar() {
  const auto surface = icosphere(1);
  const auto complex = complexOf(surface);
  const SparseMatrix identity = [&] {
    SparseMatrix matrix(complex.count(1), complex.count(1));
    matrix.setIdentity();
    return matrix;
  }();
  const SparseMatrix L = discretization::laplacian(complex, identity);
  bool valence = true;

  for (Index v = 0; v < complex.count(0); ++v) {
    valence = valence && L.coeff(v, v) == (v < 12 ? 5.0 : 6.0);
  }

  expect(valence, "any star on 1-forms works: identity gives the graph Laplacian");
}

void testStar2() {
  const auto complex = SimplicialComplex::fromTopSimplices(2, 3, std::vector<Index>{0, 1, 2});
  const std::vector<glm::dvec3> positions{{0, 0, 0}, {3, 0, 0}, {0, 4, 0}};
  const auto metric = Metric::fromPositions(complex, positions);

  expect(near(discretization::hodgeStar2(metric).coeff(0, 0), 1.0 / 6.0), "star2 is inverse area");
}

void testDegenerateTriangle() {
  const auto complex = SimplicialComplex::fromTopSimplices(2, 3, std::vector<Index>{0, 1, 2});
  const std::vector<glm::dvec3> positions{{0, 0, 0}, {1, 0, 0}, {2, 0, 0}};
  const auto metric = Metric::fromPositions(complex, positions);
  const SparseMatrix L = discretization::laplacian(complex, discretization::hodgeStar1(metric));

  expect(!std::isfinite(SparseMatrix(L).coeffs().sum()),
         "degenerate triangle: the Laplacian is not finite");
}

void testErrors() {
  const auto tetrahedron = SimplicialComplex::fromTopSimplices(3, 4,
                                                               std::vector<Index>{0, 1, 2, 3});
  const auto metric = Metric::fromEdgeLengths(tetrahedron, std::vector<double>(6, 1.0));
  const auto triangle = SimplicialComplex::fromTopSimplices(2, 3, std::vector<Index>{0, 1, 2});

  expect(throws([&] {
           discretization::hodgeStar1(metric);
         }),
         "diagonal stars reject non-surface complexes");
  expect(throws([&] {
           discretization::laplacian(triangle, SparseMatrix(2, 2));
         }),
         "Laplacian rejects a star of the wrong size");
}

} // namespace

int main() {
  testLaplacianIdentities();
  testCotanFormula();
  testDualAreas();
  testObtuseTriangle();
  testSphereConvergence();
  testGenericStar();
  testStar2();
  testDegenerateTriangle();
  testErrors();

  return failures == 0 ? 0 : 1;
}
