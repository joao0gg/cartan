#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <glm/vec3.hpp>

namespace cartan::io {

struct MeshData {
  std::vector<glm::vec3> positions;
  std::vector<std::array<std::uint32_t, 3>> triangles;
};

} // namespace cartan::io
