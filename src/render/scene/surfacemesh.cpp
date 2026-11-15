#include "render/scene/surfacemesh.h"

#include <stdexcept>
#include <utility>

#include <glm/geometric.hpp>

namespace cartan::render {

SurfaceMesh::SurfaceMesh(std::string name, std::vector<glm::vec3> positions,
                         std::vector<Triangle> triangles)
    : Structure(std::move(name)), m_positions(std::move(positions)),
      m_triangles(std::move(triangles)) {
  for (const Triangle &triangle : m_triangles) {
    for (const std::uint32_t index : triangle) {
      if (index >= m_positions.size()) {
        throw std::invalid_argument("triangle index out of range");
      }
    }
  }

  for (const glm::vec3 &position : m_positions) {
    m_bounds.extend(position);
  }

  computeNormals();
}

const std::vector<glm::vec3> &SurfaceMesh::positions() const {
  return m_positions;
}

const std::vector<SurfaceMesh::Triangle> &SurfaceMesh::triangles() const {
  return m_triangles;
}

const std::vector<glm::vec3> &SurfaceMesh::normals() const {
  return m_normals;
}

Bounds SurfaceMesh::bounds() const {
  return m_bounds;
}

void SurfaceMesh::computeNormals() {
  m_normals.assign(m_positions.size(), glm::vec3(0.0f));

  for (const Triangle &triangle : m_triangles) {
    const glm::vec3 &a = m_positions[triangle[0]];
    const glm::vec3 &b = m_positions[triangle[1]];
    const glm::vec3 &c = m_positions[triangle[2]];

    const glm::vec3 normal = glm::cross(b - a, c - a);

    for (const std::uint32_t index : triangle) {
      m_normals[index] += normal;
    }
  }

  for (glm::vec3 &normal : m_normals) {
    const float length = glm::length(normal);
    normal = length > 0.0f ? normal / length : glm::vec3(0.0f, 1.0f, 0.0f);
  }
}

} // namespace cartan::render
