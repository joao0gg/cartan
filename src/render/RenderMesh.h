#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace cartan::render {

struct RenderMesh {
  struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
  };

  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;

  glm::vec3 boundsMin{0.0f};
  glm::vec3 boundsMax{0.0f};

  void computeBounds();
  bool empty() const {
    return indices.empty();
  }

  std::size_t edgeCount() const;
};

} // namespace cartan::render