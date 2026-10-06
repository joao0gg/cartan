// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "core/Types.h"

namespace cartan::tests {

using core::Index;

struct Surface {
  std::vector<glm::dvec3> positions;
  std::vector<Index> triangles;
};

inline Surface icosphere(int subdivisions) {
  const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
  Surface surface;
  surface.positions = {{-1, phi, 0}, {1, phi, 0}, {-1, -phi, 0}, {1, -phi, 0},
                       {0, -1, phi}, {0, 1, phi}, {0, -1, -phi}, {0, 1, -phi},
                       {phi, 0, -1}, {phi, 0, 1}, {-phi, 0, -1}, {-phi, 0, 1}};
  surface.triangles = {0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
                       4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
                       6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1};

  for (int level = 0; level < subdivisions; ++level) {
    std::map<std::pair<Index, Index>, Index> midpoints;
    std::vector<Index> refined;

    const auto midpoint = [&](Index a, Index b) {
      const auto key = std::minmax(a, b);
      const auto [found,
                  inserted] = midpoints.emplace(key, static_cast<Index>(surface.positions.size()));

      if (inserted) {
        surface.positions.push_back(0.5 * (surface.positions[a] + surface.positions[b]));
      }

      return found->second;
    };

    for (std::size_t t = 0; t < surface.triangles.size(); t += 3) {
      const Index a = surface.triangles[t];
      const Index b = surface.triangles[t + 1];
      const Index c = surface.triangles[t + 2];
      const Index ab = midpoint(a, b);
      const Index bc = midpoint(b, c);
      const Index ca = midpoint(c, a);
      refined.insert(refined.end(), {a, ab, ca, b, bc, ab, c, ca, bc, ab, bc, ca});
    }

    surface.triangles = std::move(refined);
  }

  for (auto &position : surface.positions) {
    position = glm::normalize(position);
  }

  return surface;
}

} // namespace cartan::tests
