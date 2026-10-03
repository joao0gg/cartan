#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace cartan::core {

class Mesh {
public:
  using Triangle = std::array<std::uint32_t, 3>;
  using Edge = std::array<std::uint32_t, 2>;
  using TriangleEdges = std::array<std::uint32_t, 3>;
  using TriangleEdgeSigns = std::array<std::int8_t, 3>;

  static Mesh fromTriangles(std::vector<glm::dvec3> positions, std::vector<Triangle> triangles);

  const std::vector<glm::dvec3> &positions() const {
    return m_positions;
  }

  const std::vector<Triangle> &triangles() const {
    return m_triangles;
  }

  const std::vector<Edge> &edges() const {
    return m_edges;
  }

  const std::vector<TriangleEdges> &triangleEdges() const {
    return m_triangleEdges;
  }

  const std::vector<TriangleEdgeSigns> &triangleEdgeSigns() const {
    return m_triangleEdgeSigns;
  }

  bool empty() const {
    return m_triangles.empty();
  }

private:
  Mesh() = default;

  std::vector<glm::dvec3> m_positions;
  std::vector<Triangle> m_triangles;
  std::vector<Edge> m_edges;
  std::vector<TriangleEdges> m_triangleEdges;
  std::vector<TriangleEdgeSigns> m_triangleEdgeSigns;
};

struct Bounds {
  glm::dvec3 min{0.0};
  glm::dvec3 max{0.0};
};

Bounds bounds(const Mesh &mesh);

} // namespace cartan::core
