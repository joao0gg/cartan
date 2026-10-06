// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "discretization/HodgeStar.h"

#include <stdexcept>
#include <vector>

namespace cartan::discretization {

namespace {

using core::Index;

void requireSurface(const geometry::Metric &metric) {
  if (metric.complex().dimension() != 2) {
    throw std::invalid_argument("diagonal Hodge stars require a two-dimensional complex");
  }
}

core::SparseMatrix diagonal(const std::vector<double> &values) {
  const auto size = static_cast<Index>(values.size());
  std::vector<Eigen::Triplet<double, Index>> entries;
  entries.reserve(values.size());

  for (Index i = 0; i < size; ++i) {
    entries.emplace_back(i, i, values[i]);
  }

  core::SparseMatrix matrix(size, size);
  matrix.setFromTriplets(entries.begin(), entries.end());

  return matrix;
}

} // namespace

core::SparseMatrix hodgeStar0(const geometry::Metric &metric, DualArea dualArea) {
  requireSurface(metric);
  const auto &complex = metric.complex();
  std::vector<double> areas(static_cast<std::size_t>(complex.count(0)), 0.0);

  for (Index t = 0; t < complex.count(2); ++t) {
    const auto vertices = complex.simplex(2, t);

    if (dualArea == DualArea::Barycentric) {
      for (const Index vertex : vertices) {
        areas[vertex] += metric.volume(2, t) / 3.0;
      }

      continue;
    }

    const auto edges = complex.faces(2, t);

    for (int corner = 0; corner < 3; ++corner) {
      const double length = metric.volume(1, edges[corner]);
      const double share = length * length * metric.cotangent(t, corner) / 8.0;
      areas[vertices[(corner + 1) % 3]] += share;
      areas[vertices[(corner + 2) % 3]] += share;
    }
  }

  return diagonal(areas);
}

core::SparseMatrix hodgeStar1(const geometry::Metric &metric) {
  requireSurface(metric);
  const auto &complex = metric.complex();
  std::vector<double> weights(static_cast<std::size_t>(complex.count(1)), 0.0);

  for (Index t = 0; t < complex.count(2); ++t) {
    const auto edges = complex.faces(2, t);

    for (int corner = 0; corner < 3; ++corner) {
      weights[edges[corner]] += metric.cotangent(t, corner) / 2.0;
    }
  }

  return diagonal(weights);
}

core::SparseMatrix hodgeStar2(const geometry::Metric &metric) {
  requireSurface(metric);
  const auto areas = metric.volumes(2);
  std::vector<double> inverses(areas.size());

  for (std::size_t t = 0; t < areas.size(); ++t) {
    inverses[t] = 1.0 / areas[t];
  }

  return diagonal(inverses);
}

} // namespace cartan::discretization
