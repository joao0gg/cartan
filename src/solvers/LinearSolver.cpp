// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "solvers/LinearSolver.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>

namespace cartan::solvers {

namespace {

using Cholesky = Eigen::SimplicialLDLT<core::SparseMatrix>;
using LU = Eigen::SparseLU<core::SparseMatrix, Eigen::COLAMDOrdering<core::Index>>;
using ConjugateGradient = Eigen::ConjugateGradient<
    core::SparseMatrix, Eigen::Lower | Eigen::Upper,
    Eigen::IncompleteCholesky<double, Eigen::Lower, Eigen::AMDOrdering<core::Index>>>;

} // namespace

struct LinearSolver::Implementation {
  Method method;
  IterativeOptions options;
  std::variant<std::monostate, Cholesky, LU, ConjugateGradient> solver;
  core::SparseMatrix matrix;
  core::Index size = 0;
  bool computed = false;
};

LinearSolver::LinearSolver(Method method, IterativeOptions options)
    : m_implementation(std::make_unique<Implementation>()) {
  if (options.tolerance <= 0.0 || options.maxIterations < 0) {
    throw std::invalid_argument("iterative options need a positive tolerance and a "
                                "non-negative iteration limit");
  }

  m_implementation->method = method;
  m_implementation->options = options;

  switch (method) {
  case Method::Cholesky:
    m_implementation->solver.emplace<Cholesky>();
    break;
  case Method::LU:
    m_implementation->solver.emplace<LU>();
    break;
  case Method::ConjugateGradient: {
    auto &cg = m_implementation->solver.emplace<ConjugateGradient>();
    cg.setTolerance(options.tolerance);

    if (options.maxIterations > 0) {
      cg.setMaxIterations(options.maxIterations);
    }

    break;
  }
  }
}

LinearSolver::~LinearSolver() = default;
LinearSolver::LinearSolver(LinearSolver &&) noexcept = default;
LinearSolver &LinearSolver::operator=(LinearSolver &&) noexcept = default;

void LinearSolver::compute(const core::SparseMatrix &matrix) {
  if (matrix.rows() != matrix.cols()) {
    throw std::invalid_argument("the matrix must be square");
  }

  for (int column = 0; column < matrix.outerSize(); ++column) {
    for (core::SparseMatrix::InnerIterator entry(matrix, column); entry; ++entry) {
      if (!std::isfinite(entry.value())) {
        throw std::invalid_argument("the matrix has a non-finite entry at (" +
                                    std::to_string(entry.row()) + ", " +
                                    std::to_string(entry.col()) + ")");
      }
    }
  }

  m_implementation->computed = false;
  m_implementation->size = static_cast<core::Index>(matrix.rows());

  std::visit(
      [&](auto &solver) {
        using Solver = std::decay_t<decltype(solver)>;

        if constexpr (!std::is_same_v<Solver, std::monostate>) {
          if constexpr (std::is_same_v<Solver, LU>) {
            solver.analyzePattern(matrix);
            solver.factorize(matrix);

            if (solver.info() != Eigen::Success) {
              throw std::runtime_error("LU factorization failed: " + solver.lastErrorMessage());
            }
          }
          else if constexpr (std::is_same_v<Solver, ConjugateGradient>) {
            m_implementation->matrix = matrix;
            solver.compute(m_implementation->matrix);

            if (solver.info() != Eigen::Success) {
              throw std::runtime_error("the incomplete Cholesky preconditioner failed");
            }
          }
          else {
            solver.compute(matrix);

            if (solver.info() != Eigen::Success) {
              throw std::runtime_error("Cholesky factorization failed: the matrix is singular or "
                                       "not symmetric positive (semi)definite");
            }
          }
        }
      },
      m_implementation->solver);

  m_implementation->computed = true;
}

Eigen::VectorXd LinearSolver::solve(const Eigen::VectorXd &rhs) const {
  if (!m_implementation->computed) {
    throw std::logic_error("solve called before a successful compute");
  }

  if (rhs.size() != m_implementation->size) {
    throw std::invalid_argument("right-hand side has " + std::to_string(rhs.size()) +
                                " entries, the matrix has " +
                                std::to_string(m_implementation->size) + " rows");
  }

  Eigen::VectorXd solution = std::visit(
      [&](const auto &solver) -> Eigen::VectorXd {
        using Solver = std::decay_t<decltype(solver)>;

        if constexpr (std::is_same_v<Solver, std::monostate>) {
          throw std::logic_error("no solver configured");
        }
        else {
          Eigen::VectorXd result = solver.solve(rhs);

          if (solver.info() != Eigen::Success) {
            if constexpr (std::is_same_v<Solver, ConjugateGradient>) {
              throw std::runtime_error("conjugate gradient did not converge after " +
                                       std::to_string(solver.iterations()) +
                                       " iterations (relative residual " +
                                       std::to_string(solver.error()) + ")");
            }
            else {
              throw std::runtime_error("the solve failed");
            }
          }

          return result;
        }
      },
      m_implementation->solver);

  if (!solution.allFinite()) {
    throw std::runtime_error("the solution is not finite; the matrix is likely singular");
  }

  return solution;
}

Method LinearSolver::method() const {
  return m_implementation->method;
}

int LinearSolver::iterations() const {
  if (const auto *cg = std::get_if<ConjugateGradient>(&m_implementation->solver)) {
    return static_cast<int>(cg->iterations());
  }

  return 0;
}

double LinearSolver::error() const {
  if (const auto *cg = std::get_if<ConjugateGradient>(&m_implementation->solver)) {
    return cg->error();
  }

  return 0.0;
}

} // namespace cartan::solvers
