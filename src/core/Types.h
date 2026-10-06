// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <cstdint>

#include <Eigen/SparseCore>

namespace cartan::core {

using Index = std::int32_t;

using SparseMatrix = Eigen::SparseMatrix<double, Eigen::ColMajor, Index>;

} // namespace cartan::core
