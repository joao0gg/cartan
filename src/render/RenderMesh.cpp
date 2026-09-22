#include "render/RenderMesh.h"

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace cartan::render {

void RenderMesh::computeBounds() {
  if (vertices.empty()) {
    boundsMin = glm::vec3(0.0f);
    boundsMax = glm::vec3(0.0f);

    return;
  }

  boundsMin = glm::vec3(std::numeric_limits<float>::max());
  boundsMax = glm::vec3(std::numeric_limits<float>::lowest());
  for (const auto &vertex : vertices) {
    boundsMin = glm::min(boundsMin, vertex.position);
    boundsMax = glm::max(boundsMax, vertex.position);
  }
}

std::size_t RenderMesh::edgeCount() const {
  std::unordered_set<std::uint64_t> edges;
  edges.reserve(indices.size());

  for (std::size_t i = 0; i + 2 < indices.size(); i += 3) {
    for (std::size_t e = 0; e < 3; ++e) {
      const std::uint64_t a = indices[i + e];
      const std::uint64_t b = indices[i + (e + 1) % 3];

      edges.insert((std::min(a, b) << 32) | std::max(a, b));
    }
  }

  return edges.size();
}

} // namespace cartan::render