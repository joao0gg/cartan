// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "core/SimplicialComplex.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>

#include "core/detail/FlatTable.h"

namespace cartan::core {

namespace {

template <std::size_t N> int parity(const std::array<Index, N> &vertices) {
  int inversions = 0;

  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = i + 1; j < N; ++j) {
      inversions += vertices[i] > vertices[j] ? 1 : 0;
    }
  }

  return inversions % 2 == 0 ? 1 : -1;
}

template <std::size_t N>
void checkTopSimplices(std::span<const Index> vertices, Index vertexCount) {
  const std::size_t topCount = vertices.size() / N;
  detail::FlatTable<N> seen(topCount);

  for (std::size_t t = 0; t < topCount; ++t) {
    std::array<Index, N> key;
    std::copy_n(vertices.begin() + static_cast<std::ptrdiff_t>(t * N), N, key.begin());

    for (const Index vertex : key) {
      if (vertex < 0 || vertex >= vertexCount) {
        throw std::invalid_argument("simplex " + std::to_string(t) + " references vertex " +
                                    std::to_string(vertex) + " outside [0, " +
                                    std::to_string(vertexCount) + ")");
      }
    }

    std::ranges::sort(key);

    if (std::ranges::adjacent_find(key) != key.end()) {
      throw std::invalid_argument("simplex " + std::to_string(t) + " repeats a vertex");
    }

    const auto [existing, inserted] = seen.insert(key, static_cast<Index>(t));

    if (!inserted) {
      throw std::invalid_argument("simplex " + std::to_string(t) + " duplicates simplex " +
                                  std::to_string(existing));
    }
  }
}

template <std::size_t FaceSize>
void buildFaces(std::span<const Index> cofaces, std::vector<Index> &faces, SparseMatrix &d,
                Index faceCount) {
  constexpr std::size_t cofaceSize = FaceSize + 1;
  const std::size_t cofaceCount = cofaces.size() / cofaceSize;
  detail::FlatTable<FaceSize> table(FaceSize == 1 ? 0 : cofaceCount * cofaceSize / 2);
  std::vector<Eigen::Triplet<double, Index>> entries;
  entries.reserve(cofaces.size());

  for (std::size_t s = 0; s < cofaceCount; ++s) {
    const auto coface = cofaces.subspan(s * cofaceSize, cofaceSize);

    for (std::size_t omitted = 0; omitted < cofaceSize; ++omitted) {
      std::array<Index, FaceSize> face;
      std::size_t size = 0;

      for (std::size_t v = 0; v < cofaceSize; ++v) {
        if (v != omitted) {
          face[size++] = coface[v];
        }
      }

      const int sign = (omitted % 2 == 0 ? 1 : -1) * parity(face);
      Index index = face[0];

      if constexpr (FaceSize > 1) {
        std::ranges::sort(face);
        const auto next = static_cast<Index>(faces.size() / FaceSize);
        const auto [found, inserted] = table.insert(face, next);

        if (inserted) {
          faces.insert(faces.end(), face.begin(), face.end());
        }

        index = found;
      }

      entries.emplace_back(static_cast<Index>(s), index, static_cast<double>(sign));
    }
  }

  if constexpr (FaceSize > 1) {
    faceCount = static_cast<Index>(faces.size() / FaceSize);
  }

  d.resize(static_cast<Index>(cofaceCount), faceCount);
  d.setFromTriplets(entries.begin(), entries.end());
}

} // namespace

SimplicialComplex SimplicialComplex::fromTopSimplices(int dimension, Index vertexCount,
                                                      std::span<const Index> vertices) {
  if (dimension < 1 || dimension > maxDimension) {
    throw std::invalid_argument("dimension must be between 1 and " + std::to_string(maxDimension));
  }

  if (vertexCount < 0) {
    throw std::invalid_argument("negative vertex count");
  }

  const std::size_t stride = static_cast<std::size_t>(dimension) + 1;

  if (vertices.size() % stride != 0) {
    throw std::invalid_argument("vertex list length is not a multiple of dimension + 1");
  }

  if (vertices.size() > static_cast<std::size_t>(std::numeric_limits<Index>::max())) {
    throw std::invalid_argument("too many simplices for the index type");
  }

  switch (dimension) {
  case 1:
    checkTopSimplices<2>(vertices, vertexCount);
    break;
  case 2:
    checkTopSimplices<3>(vertices, vertexCount);
    break;
  default:
    checkTopSimplices<4>(vertices, vertexCount);
    break;
  }

  SimplicialComplex complex;
  complex.m_dimension = dimension;
  complex.m_simplices[0].resize(static_cast<std::size_t>(vertexCount));
  std::iota(complex.m_simplices[0].begin(), complex.m_simplices[0].end(), Index{0});
  complex.m_simplices[dimension].assign(vertices.begin(), vertices.end());

  for (int k = dimension - 1; k >= 0; --k) {
    const std::span<const Index> cofaces = complex.m_simplices[k + 1];
    auto &faces = complex.m_simplices[k];

    switch (k) {
    case 0:
      buildFaces<1>(cofaces, faces, complex.m_d[0], vertexCount);
      break;
    case 1:
      buildFaces<2>(cofaces, faces, complex.m_d[1], 0);
      break;
    default:
      buildFaces<3>(cofaces, faces, complex.m_d[2], 0);
      break;
    }
  }

  return complex;
}

int SimplicialComplex::dimension() const {
  return m_dimension;
}

Index SimplicialComplex::count(int k) const {
  checkDimension(k);

  return static_cast<Index>(m_simplices[k].size() / (static_cast<std::size_t>(k) + 1));
}

std::span<const Index> SimplicialComplex::simplex(int k, Index i) const {
  checkDimension(k);
  const std::size_t size = static_cast<std::size_t>(k) + 1;

  return std::span(m_simplices[k]).subspan(static_cast<std::size_t>(i) * size, size);
}

const SparseMatrix &SimplicialComplex::d(int k) const {
  if (k < 0 || k >= m_dimension) {
    throw std::out_of_range("d(" + std::to_string(k) +
                            ") does not exist in a complex of dimension " +
                            std::to_string(m_dimension));
  }

  return m_d[k];
}

int SimplicialComplex::eulerCharacteristic() const {
  int characteristic = 0;

  for (int k = 0; k <= m_dimension; ++k) {
    characteristic += (k % 2 == 0 ? 1 : -1) * count(k);
  }

  return characteristic;
}

void SimplicialComplex::checkDimension(int k) const {
  if (k < 0 || k > m_dimension) {
    throw std::out_of_range("no " + std::to_string(k) + "-simplices in a complex of dimension " +
                            std::to_string(m_dimension));
  }
}

} // namespace cartan::core
