// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include "core/SimplicialComplex.h"
#include "core/Types.h"

namespace cartan::discretization {

core::SparseMatrix laplacian(const core::SimplicialComplex &complex,
                             const core::SparseMatrix &star1);

} // namespace cartan::discretization
