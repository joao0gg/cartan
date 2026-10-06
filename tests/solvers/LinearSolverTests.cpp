// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <cmath>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "core/SimplicialComplex.h"
#include "discretization/HodgeStar.h"
#include "discretization/Laplacian.h"
#include "geometry/Metric.h"
#include "solvers/LinearSolver.h"
#include "support/Icosphere.h"

namespace {

using cartan::core::Index;
using cartan::core::SimplicialComplex;
using cartan::core::SparseMatrix;
using cartan::discretization::DualArea;
using cartan::geometry::Metric;
using cartan::solvers::IterativeOptions;
using cartan::solvers::LinearSolver;
using cartan::solvers::Method;
using cartan::tests::icosphere;

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

SparseMatrix sparse(const Eigen::MatrixXd &dense) {
  return dense.sparseView();
}

constexpr Method allMethods[] = {Method::Cholesky, Method::LU, Method::ConjugateGradient};

void testSmallSystem() {
  Eigen::MatrixXd dense(3, 3);
  dense << 4, 1, 0, 1, 3, -1, 0, -1, 2;
  const Eigen::VectorXd expected = Eigen::Vector3d(1.0, -2.0, 0.5);
  const Eigen::VectorXd rhs = dense * expected;

  for (const Method method : allMethods) {
    LinearSolver solver(method);
    solver.compute(sparse(dense));
    expect((solver.solve(rhs) - expected).norm() < 1e-9,
           "small SPD system: every method solves it");
  }
}

void testScreenedPoissonOnSphere() {
  const auto surface = icosphere(4);
  const auto complex = SimplicialComplex::fromTopSimplices(2,
                                                           static_cast<Index>(
                                                               surface.positions.size()),
                                                           surface.triangles);
  const auto metric = Metric::fromPositions(complex, surface.positions);
  const SparseMatrix star0 = discretization::hodgeStar0(metric, DualArea::Circumcentric);
  const SparseMatrix L = discretization::laplacian(complex, discretization::hodgeStar1(metric));
  const SparseMatrix system = L + star0;
  Eigen::VectorXd z(complex.count(0));

  for (Index v = 0; v < complex.count(0); ++v) {
    z[v] = surface.positions[v].z;
  }

  const Eigen::VectorXd rhs = star0 * z;
  const Eigen::VectorXd exact = z / 3.0;

  for (const Method method : allMethods) {
    LinearSolver solver(method);
    solver.compute(system);
    const Eigen::VectorXd solution = solver.solve(rhs);
    expect((solution - exact).lpNorm<Eigen::Infinity>() < 2e-3,
           "sphere: (L + M) u = M z gives u = z / 3");
  }

  LinearSolver cg(Method::ConjugateGradient);
  cg.compute(system);
  cg.solve(rhs);
  expect(cg.iterations() > 0 && cg.error() < 1e-10, "conjugate gradient reports its convergence");

  LinearSolver direct(Method::Cholesky);
  direct.compute(system);
  direct.solve(rhs);
  expect(direct.iterations() == 0 && direct.error() == 0.0, "direct methods report no iterations");
}

void testReuseAndMove() {
  Eigen::MatrixXd dense(2, 2);
  dense << 2, 1, 1, 3;
  LinearSolver solver(Method::Cholesky);
  solver.compute(sparse(dense));

  const Eigen::VectorXd first = solver.solve(Eigen::Vector2d(1.0, 0.0));
  const Eigen::VectorXd second = solver.solve(Eigen::Vector2d(0.0, 1.0));
  expect((dense * first - Eigen::Vector2d(1.0, 0.0)).norm() < 1e-12 &&
             (dense * second - Eigen::Vector2d(0.0, 1.0)).norm() < 1e-12,
         "one factorization serves several right-hand sides");

  LinearSolver moved = std::move(solver);
  expect((dense * moved.solve(Eigen::Vector2d(1.0, 1.0)) - Eigen::Vector2d(1.0, 1.0)).norm() <
             1e-12,
         "a moved solver keeps its factorization");
}

void testSolverOwnsItsMatrix() {
  LinearSolver solver(Method::ConjugateGradient);
  Eigen::MatrixXd dense(2, 2);
  dense << 2, 1, 1, 3;

  {
    const SparseMatrix temporary = sparse(dense);
    solver.compute(temporary);
  }

  LinearSolver moved = std::move(solver);
  const Eigen::VectorXd solution = moved.solve(Eigen::Vector2d(1.0, 2.0));
  expect((dense * solution - Eigen::Vector2d(1.0, 2.0)).norm() < 1e-9,
         "conjugate gradient keeps working after the input matrix is gone and the solver moved");
}

void testNonsymmetricLU() {
  Eigen::MatrixXd dense(3, 3);
  dense << 1, 2, 0, 0, 1, 3, 4, 0, 1;
  const Eigen::VectorXd expected = Eigen::Vector3d(1.0, 1.0, 1.0);
  LinearSolver solver(Method::LU);
  solver.compute(sparse(dense));

  expect((solver.solve(dense * expected) - expected).norm() < 1e-12,
         "LU solves nonsymmetric systems");
}

void testErrors() {
  Eigen::MatrixXd singular(2, 2);
  singular << 1, 1, 1, 1;
  Eigen::MatrixXd poisoned(2, 2);
  poisoned << 1, 0, 0, std::numeric_limits<double>::infinity();

  expect(throws([] {
           LinearSolver(Method::Cholesky).solve(Eigen::Vector2d(1, 1));
         }),
         "solve before compute throws");
  expect(throws([] {
           LinearSolver(Method::Cholesky).compute(SparseMatrix(2, 3));
         }),
         "non-square matrix throws");
  expect(throws([&] {
           LinearSolver(Method::Cholesky).compute(sparse(poisoned));
         }),
         "non-finite matrix throws");
  expect(throws([&] {
           LinearSolver(Method::Cholesky).compute(sparse(singular));
         }),
         "singular matrix throws in Cholesky");
  expect(throws([&] {
           LinearSolver(Method::LU).compute(sparse(singular));
         }),
         "singular matrix throws in LU");
  expect(throws([&] {
           LinearSolver solver(Method::Cholesky);
           solver.compute(sparse(Eigen::MatrixXd::Identity(2, 2)));
           solver.solve(Eigen::Vector3d(1, 1, 1));
         }),
         "right-hand side of the wrong size throws");
  expect(throws([] {
           LinearSolver(Method::ConjugateGradient, IterativeOptions{-1.0, 0});
         }),
         "invalid iterative options throw");

  const auto surface = icosphere(3);
  const auto complex = SimplicialComplex::fromTopSimplices(2,
                                                           static_cast<Index>(
                                                               surface.positions.size()),
                                                           surface.triangles);
  const auto metric = Metric::fromPositions(complex, surface.positions);
  const SparseMatrix system = discretization::laplacian(complex,
                                                        discretization::hodgeStar1(metric)) +
                              discretization::hodgeStar0(metric, DualArea::Barycentric);
  LinearSolver limited(Method::ConjugateGradient, IterativeOptions{1e-14, 1});
  limited.compute(system);

  expect(throws([&] {
           limited.solve(Eigen::VectorXd::Random(complex.count(0)));
         }),
         "conjugate gradient that runs out of iterations throws");
}

} // namespace

int main() {
  testSmallSystem();
  testScreenedPoissonOnSphere();
  testReuseAndMove();
  testSolverOwnsItsMatrix();
  testNonsymmetricLU();
  testErrors();

  return failures == 0 ? 0 : 1;
}
