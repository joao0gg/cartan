// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include "core/Types.h"
#include "geometry/Metric.h"

namespace cartan::discretization {

enum class DualArea { Barycentric, Circumcentric };

core::SparseMatrix hodgeStar0(const geometry::Metric &metric, DualArea dualArea);
core::SparseMatrix hodgeStar1(const geometry::Metric &metric);
core::SparseMatrix hodgeStar2(const geometry::Metric &metric);

} // namespace cartan::discretization
