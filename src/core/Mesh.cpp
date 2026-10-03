#include "core/Mesh.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace cartan::core {

Mesh Mesh::fromTriangles(std::vector<glm::dvec3> positions, std::vector<Triangle> triangles) {
  const std::size_t vertexCount = positions.size();

  for (std::size_t t = 0; t < triangles.size(); ++t) {
    const Triangle &triangle = triangles[t];

    for (const std::uint32_t vertex : triangle) {
      if (vertex >= vertexCount) {
        throw std::invalid_argument("triangle " + std::to_string(t) + " references vertex " +
                                    std::to_string(vertex) + " out of range");
      }
    }

    if (triangle[0] == triangle[1] || triangle[1] == triangle[2] || triangle[2] == triangle[0]) {
      throw std::invalid_argument("triangle " + std::to_string(t) + " is degenerate");
    }
  }

  Mesh mesh;
  mesh.m_positions = std::move(positions);
  mesh.m_triangles = std::move(triangles);
  mesh.m_triangleEdges.reserve(mesh.m_triangles.size());
  mesh.m_triangleEdgeSigns.reserve(mesh.m_triangles.size());

  std::unordered_map<std::uint64_t, std::uint32_t> edgeIndex;
  edgeIndex.reserve(mesh.m_triangles.size() * 2);

  for (const Triangle &triangle : mesh.m_triangles) {
    TriangleEdges triangleEdges{};
    TriangleEdgeSigns triangleEdgeSigns{};

    for (std::size_t k = 0; k < 3; ++k) {
      const std::uint32_t from = triangle[k];
      const std::uint32_t to = triangle[(k + 1) % 3];
      const std::uint32_t low = std::min(from, to);
      const std::uint32_t high = std::max(from, to);

      const std::uint64_t key = (static_cast<std::uint64_t>(low) << 32) | high;
      const auto [entry, inserted] = edgeIndex.try_emplace(key, static_cast<std::uint32_t>(
                                                                    mesh.m_edges.size()));

      if (inserted) {
        mesh.m_edges.push_back({low, high});
      }

      triangleEdges[k] = entry->second;
      triangleEdgeSigns[k] = from < to ? 1 : -1;
    }

    mesh.m_triangleEdges.push_back(triangleEdges);
    mesh.m_triangleEdgeSigns.push_back(triangleEdgeSigns);
  }

  return mesh;
}

Bounds bounds(const Mesh &mesh) {
  const auto &positions = mesh.positions();

  if (positions.empty()) {
    return {};
  }

  Bounds result{glm::dvec3(std::numeric_limits<double>::max()),
                glm::dvec3(std::numeric_limits<double>::lowest())};

  for (const auto &position : positions) {
    result.min = glm::min(result.min, position);
    result.max = glm::max(result.max, position);
  }

  return result;
}

} // namespace cartan::core
