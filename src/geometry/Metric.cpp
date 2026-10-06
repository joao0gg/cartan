// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "geometry/Metric.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <string>

#include <Eigen/Dense>
#include <glm/geometric.hpp>

namespace cartan::geometry {

namespace {

using core::Index;
using core::SimplicialComplex;

double triangleArea(double a, double b, double c) {
  std::array<double, 3> sides{a, b, c};
  std::ranges::sort(sides, std::greater<>());
  const auto [x, y, z] = sides;
  const double product = (x + (y + z)) * (z - (x - y)) * (z + (x - y)) * (x + (y - z));

  return 0.25 * std::sqrt(std::max(product, 0.0));
}

double tetrahedronVolumeSquared(const SimplicialComplex &complex, std::span<const double> lengths,
                                Index tetrahedron) {
  const auto vertices = complex.simplex(3, tetrahedron);
  Eigen::Matrix<double, 5, 5> cayleyMenger = Eigen::Matrix<double, 5, 5>::Ones();
  cayleyMenger.diagonal().setZero();

  const auto local = [&](Index vertex) {
    return static_cast<Index>(std::ranges::find(vertices, vertex) - vertices.begin()) + 1;
  };

  for (const Index triangle : complex.faces(3, tetrahedron)) {
    for (const Index edge : complex.faces(2, triangle)) {
      const auto ends = complex.simplex(1, edge);
      const double squared = lengths[edge] * lengths[edge];
      cayleyMenger(local(ends[0]), local(ends[1])) = squared;
      cayleyMenger(local(ends[1]), local(ends[0])) = squared;
    }
  }

  return cayleyMenger.determinant() / 288.0;
}

} // namespace

Metric Metric::fromPositions(const SimplicialComplex &complex,
                             std::span<const glm::dvec3> positions) {
  if (positions.size() != static_cast<std::size_t>(complex.count(0))) {
    throw std::invalid_argument("expected " + std::to_string(complex.count(0)) +
                                " positions, got " + std::to_string(positions.size()));
  }

  std::vector<double> lengths(static_cast<std::size_t>(complex.count(1)));

  for (Index e = 0; e < complex.count(1); ++e) {
    const auto ends = complex.simplex(1, e);
    lengths[e] = glm::distance(positions[ends[0]], positions[ends[1]]);
  }

  return Metric(complex, std::move(lengths), false);
}

Metric Metric::fromEdgeLengths(const SimplicialComplex &complex, std::vector<double> lengths) {
  if (lengths.size() != static_cast<std::size_t>(complex.count(1))) {
    throw std::invalid_argument("expected " + std::to_string(complex.count(1)) +
                                " edge lengths, got " + std::to_string(lengths.size()));
  }

  for (std::size_t e = 0; e < lengths.size(); ++e) {
    if (!std::isfinite(lengths[e]) || lengths[e] < 0.0) {
      throw std::invalid_argument("edge " + std::to_string(e) + " has invalid length " +
                                  std::to_string(lengths[e]));
    }
  }

  return Metric(complex, std::move(lengths), true);
}

Metric::Metric(const SimplicialComplex &complex, std::vector<double> lengths, bool validate)
    : m_complex(&complex) {
  m_volumes[0].assign(static_cast<std::size_t>(complex.count(0)), 1.0);

  if (complex.dimension() >= 2) {
    m_volumes[2].resize(static_cast<std::size_t>(complex.count(2)));

    for (Index t = 0; t < complex.count(2); ++t) {
      const auto edges = complex.faces(2, t);
      const double a = lengths[edges[0]];
      const double b = lengths[edges[1]];
      const double c = lengths[edges[2]];

      if (validate && (a > b + c || b > a + c || c > a + b)) {
        throw std::invalid_argument("triangle " + std::to_string(t) +
                                    " violates the triangle inequality");
      }

      m_volumes[2][t] = triangleArea(a, b, c);
    }
  }

  if (complex.dimension() >= 3) {
    m_volumes[3].resize(static_cast<std::size_t>(complex.count(3)));

    for (Index t = 0; t < complex.count(3); ++t) {
      const double squared = tetrahedronVolumeSquared(complex, lengths, t);

      if (validate) {
        double longest = 0.0;

        for (const Index triangle : complex.faces(3, t)) {
          for (const Index edge : complex.faces(2, triangle)) {
            longest = std::max(longest, lengths[edge]);
          }
        }

        if (squared < -1e-12 * std::pow(longest, 6)) {
          throw std::invalid_argument("tetrahedron " + std::to_string(t) +
                                      " cannot be embedded with these edge lengths");
        }
      }

      m_volumes[3][t] = std::sqrt(std::max(squared, 0.0));
    }
  }

  m_volumes[1] = std::move(lengths);
}

const SimplicialComplex &Metric::complex() const {
  return *m_complex;
}

std::span<const double> Metric::volumes(int k) const {
  if (k < 0 || k > m_complex->dimension()) {
    throw std::out_of_range("no " + std::to_string(k) + "-simplices in a complex of dimension " +
                            std::to_string(m_complex->dimension()));
  }

  return m_volumes[k];
}

double Metric::volume(int k, Index i) const {
  return volumes(k)[static_cast<std::size_t>(i)];
}

double Metric::cotangent(Index triangle, int corner) const {
  if (corner < 0 || corner > 2) {
    throw std::out_of_range("triangle corner must be 0, 1 or 2");
  }

  const auto edges = m_complex->faces(2, triangle);
  const auto &lengths = m_volumes[1];
  const double a = lengths[edges[corner]];
  const double b = lengths[edges[(corner + 1) % 3]];
  const double c = lengths[edges[(corner + 2) % 3]];

  return (b * b + c * c - a * a) / (4.0 * m_volumes[2][triangle]);
}

} // namespace cartan::geometry
