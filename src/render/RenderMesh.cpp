#include "render/RenderMesh.h"

namespace cartan::render {

RenderMesh::RenderMesh(const core::Mesh &mesh) {
  const auto &positions = mesh.positions();
  const auto &triangles = mesh.triangles();

  std::vector<glm::dvec3> normals(positions.size(), glm::dvec3(0.0));

  for (const auto &triangle : triangles) {
    const glm::dvec3 &a = positions[triangle[0]];
    const glm::dvec3 &b = positions[triangle[1]];
    const glm::dvec3 &c = positions[triangle[2]];
    const glm::dvec3 areaNormal = glm::cross(b - a, c - a);

    for (const std::uint32_t vertex : triangle) {
      normals[vertex] += areaNormal;
    }
  }

  vertices.resize(positions.size());

  for (std::size_t v = 0; v < positions.size(); ++v) {
    const double length = glm::length(normals[v]);

    vertices[v].position = glm::vec3(positions[v]);
    vertices[v].normal = length > 0.0 ? glm::vec3(normals[v] / length) : glm::vec3(0.0f);
  }

  indices.reserve(triangles.size() * 3);

  for (const auto &triangle : triangles) {
    indices.insert(indices.end(), triangle.begin(), triangle.end());
  }
}

} // namespace cartan::render
