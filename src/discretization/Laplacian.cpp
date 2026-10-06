// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "discretization/Laplacian.h"

#include <stdexcept>

namespace cartan::discretization {

core::SparseMatrix laplacian(const core::SimplicialComplex &complex,
                             const core::SparseMatrix &star1) {
  const core::SparseMatrix &d0 = complex.d(0);

  if (star1.rows() != d0.rows() || star1.cols() != d0.rows()) {
    throw std::invalid_argument("the Hodge star on 1-forms must be square with one row per edge");
  }

  return core::SparseMatrix(d0.transpose() * star1 * d0);
}

} // namespace cartan::discretization
