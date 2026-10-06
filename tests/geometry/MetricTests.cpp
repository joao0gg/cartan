// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <cmath>
#include <cstdio>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <glm/vec3.hpp>

#include "core/SimplicialComplex.h"
#include "geometry/Metric.h"

namespace {

using cartan::core::Index;
using cartan::core::SimplicialComplex;
using cartan::geometry::Metric;

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

bool near(double a, double b, double tolerance = 1e-14) {
  return std::abs(a - b) <= tolerance * std::max(1.0, std::abs(b));
}

SimplicialComplex build(int dimension, Index vertexCount, const std::vector<Index> &vertices) {
  return SimplicialComplex::fromTopSimplices(dimension, vertexCount, vertices);
}

template <class Length>
std::vector<double> lengthsByPair(const SimplicialComplex &complex, Length length) {
  std::vector<double> lengths;

  for (Index e = 0; e < complex.count(1); ++e) {
    const auto ends = complex.simplex(1, e);
    lengths.push_back(length(ends[0], ends[1]));
  }

  return lengths;
}

double total(std::span<const double> values) {
  return std::accumulate(values.begin(), values.end(), 0.0);
}

void testRightTriangle() {
  const auto complex = build(2, 3, {0, 1, 2});
  const std::vector<glm::dvec3> positions{{0, 0, 0}, {3, 0, 0}, {0, 4, 0}};
  const auto metric = Metric::fromPositions(complex, positions);

  expect(near(total(metric.volumes(1)), 12.0), "right triangle: edge lengths 3, 4, 5");
  expect(near(metric.volume(2, 0), 6.0), "right triangle: area 6");
  expect(near(metric.cotangent(0, 0), 0.0), "right triangle: right angle has cotangent 0");
  expect(near(metric.cotangent(0, 1), 3.0 / 4.0), "right triangle: cotangent 3/4");
  expect(near(metric.cotangent(0, 2), 4.0 / 3.0), "right triangle: cotangent 4/3");
}

void testIntrinsicMatchesExtrinsic() {
  const auto complex = build(2, 4, {0, 1, 2, 0, 2, 3});
  const std::vector<glm::dvec3> positions{{0.1, 0.2, 0.3},
                                          {1.7, -0.4, 0.2},
                                          {0.9, 1.3, -0.8},
                                          {-0.6, 0.8, 0.5}};
  const auto extrinsic = Metric::fromPositions(complex, positions);
  const auto lengths = extrinsic.volumes(1);
  const auto intrinsic = Metric::fromEdgeLengths(complex, std::vector<double>(lengths.begin(),
                                                                              lengths.end()));
  bool identical = true;

  for (Index t = 0; t < complex.count(2); ++t) {
    identical = identical && intrinsic.volume(2, t) == extrinsic.volume(2, t);

    for (int corner = 0; corner < 3; ++corner) {
      identical = identical && intrinsic.cotangent(t, corner) == extrinsic.cotangent(t, corner);
    }
  }

  expect(identical, "lengths alone reproduce areas and cotangents exactly");
}

void testOctahedron() {
  const auto complex = build(2, 6, {0, 2, 4, 2, 1, 4, 1, 3, 4, 3, 0, 4,
                                    2, 0, 5, 1, 2, 5, 3, 1, 5, 0, 3, 5});
  const std::vector<glm::dvec3> positions{{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                          {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
  const auto metric = Metric::fromPositions(complex, positions);
  bool equilateral = true;

  for (Index t = 0; t < complex.count(2); ++t) {
    for (int corner = 0; corner < 3; ++corner) {
      equilateral = equilateral && near(metric.cotangent(t, corner), 1.0 / std::sqrt(3.0));
    }
  }

  expect(near(total(metric.volumes(2)), 4.0 * std::sqrt(3.0)), "octahedron: total area 4 sqrt 3");
  expect(equilateral, "octahedron: every angle is 60 degrees");
}

void testThinTriangle() {
  const double base = 1e-9;
  const auto complex = build(2, 3, {0, 1, 2});
  const auto metric = Metric::fromEdgeLengths(complex, {1.0, 1.0, base});
  const double exact = 0.5 * base * std::sqrt(1.0 - base * base / 4.0);

  expect(near(metric.volume(2, 0), exact, 1e-12), "thin triangle: area accurate to rounding");
}

void testDegenerateTriangle() {
  const auto complex = build(2, 3, {0, 1, 2});
  const std::vector<glm::dvec3> positions{{0, 0, 0}, {1, 0, 0}, {2, 0, 0}};
  const auto metric = Metric::fromPositions(complex, positions);

  expect(metric.volume(2, 0) == 0.0, "collinear triangle: area exactly 0");
  expect(!std::isfinite(metric.cotangent(0, 0)), "collinear triangle: cotangent is not finite");
}

void testTetrahedron() {
  const auto complex = build(3, 4, {0, 1, 2, 3});
  const std::vector<glm::dvec3> positions{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
  const auto metric = Metric::fromPositions(complex, positions);

  expect(near(metric.volume(3, 0), 1.0 / 6.0), "unit tetrahedron: volume 1/6");
  expect(near(total(metric.volumes(2)), 1.5 + std::sqrt(3.0) / 2.0),
         "unit tetrahedron: total face area");

  const auto regular = Metric::fromEdgeLengths(complex, std::vector<double>(6, 1.0));
  expect(near(regular.volume(3, 0), 1.0 / (6.0 * std::sqrt(2.0)), 1e-12),
         "regular tetrahedron: volume from lengths");
}

void testErrors() {
  const auto triangle = build(2, 3, {0, 1, 2});
  const auto tetrahedron = build(3, 4, {0, 1, 2, 3});

  expect(throws([&] {
           Metric::fromEdgeLengths(triangle, {1.0, 1.0});
         }),
         "wrong length count");
  expect(throws([&] {
           Metric::fromEdgeLengths(triangle, {1.0, 1.0, -1.0});
         }),
         "negative length");
  expect(throws([&] {
           Metric::fromEdgeLengths(triangle, {1.0, 1.0, 3.0});
         }),
         "triangle inequality violation");
  expect(throws([&] {
           Metric::fromPositions(triangle, std::vector<glm::dvec3>(2));
         }),
         "wrong position count");
  const auto embeddable = lengthsByPair(tetrahedron, [](Index a, Index b) {
    return a == 0 && b == 1 ? 1.2 : 1.0;
  });
  const auto impossible = lengthsByPair(tetrahedron, [](Index, Index b) {
    return b == 3 ? 1.1 : 2.0;
  });

  expect(!throws([&] {
    Metric::fromEdgeLengths(tetrahedron, embeddable);
  }),
         "embeddable tetrahedron is accepted");
  expect(throws([&] {
           Metric::fromEdgeLengths(tetrahedron, impossible);
         }),
         "tetrahedron with valid faces but no embedding throws");
  expect(throws([&] {
           Metric::fromEdgeLengths(triangle, {1.0, 1.0, 1.0}).cotangent(0, 3);
         }),
         "invalid corner throws");
}

} // namespace

int main() {
  testRightTriangle();
  testIntrinsicMatchesExtrinsic();
  testOctahedron();
  testThinTriangle();
  testDegenerateTriangle();
  testTetrahedron();
  testErrors();

  return failures == 0 ? 0 : 1;
}
