// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <memory>

#include <Eigen/Core>

#include "core/Types.h"

namespace cartan::solvers {

enum class Method { Cholesky, LU, ConjugateGradient };

struct IterativeOptions {
  double tolerance = 1e-10;
  int maxIterations = 0;
};

class LinearSolver {
public:
  explicit LinearSolver(Method method, IterativeOptions options = {});
  ~LinearSolver();

  LinearSolver(LinearSolver &&) noexcept;
  LinearSolver &operator=(LinearSolver &&) noexcept;
  LinearSolver(const LinearSolver &) = delete;
  LinearSolver &operator=(const LinearSolver &) = delete;

  void compute(const core::SparseMatrix &matrix);
  Eigen::VectorXd solve(const Eigen::VectorXd &rhs) const;

  Method method() const;
  int iterations() const;
  double error() const;

private:
  struct Implementation;
  std::unique_ptr<Implementation> m_implementation;
};

} // namespace cartan::solvers
