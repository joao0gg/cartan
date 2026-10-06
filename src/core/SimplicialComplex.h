// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <array>
#include <span>
#include <vector>

#include "core/Types.h"

namespace cartan::core {

class SimplicialComplex {
public:
  static constexpr int maxDimension = 3;

  static SimplicialComplex fromTopSimplices(int dimension, Index vertexCount,
                                            std::span<const Index> vertices);

  int dimension() const;
  Index count(int k) const;
  std::span<const Index> simplex(int k, Index i) const;
  std::span<const Index> faces(int k, Index i) const;

  const SparseMatrix &d(int k) const;

  int eulerCharacteristic() const;

private:
  SimplicialComplex() = default;

  void checkDimension(int k) const;

  int m_dimension = 0;
  std::array<std::vector<Index>, maxDimension + 1> m_simplices;
  std::array<std::vector<Index>, maxDimension + 1> m_faces;
  std::array<SparseMatrix, maxDimension> m_d;
};

} // namespace cartan::core
