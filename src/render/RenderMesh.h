// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include "core/Mesh.h"

namespace cartan::render {

struct RenderMesh {
  struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
  };

  explicit RenderMesh(const core::Mesh &mesh);

  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;
};

} // namespace cartan::render
