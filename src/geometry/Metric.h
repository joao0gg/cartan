// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <array>
#include <span>
#include <vector>

#include <glm/vec3.hpp>

#include "core/SimplicialComplex.h"

namespace cartan::geometry {

class Metric {
public:
  static Metric fromPositions(const core::SimplicialComplex &complex,
                              std::span<const glm::dvec3> positions);
  static Metric fromPositions(const core::SimplicialComplex &&,
                              std::span<const glm::dvec3>) = delete;

  static Metric fromEdgeLengths(const core::SimplicialComplex &complex,
                                std::vector<double> lengths);
  static Metric fromEdgeLengths(const core::SimplicialComplex &&, std::vector<double>) = delete;

  const core::SimplicialComplex &complex() const;

  std::span<const double> volumes(int k) const;
  double volume(int k, core::Index i) const;
  double cotangent(core::Index triangle, int corner) const;

private:
  Metric(const core::SimplicialComplex &complex, std::vector<double> lengths, bool validate);

  const core::SimplicialComplex *m_complex;
  std::array<std::vector<double>, core::SimplicialComplex::maxDimension + 1> m_volumes;
};

} // namespace cartan::geometry
