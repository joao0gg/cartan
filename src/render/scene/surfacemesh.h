#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <glm/vec3.hpp>

#include "render/scene/structure.h"

namespace cartan::render {

struct SurfaceMeshOptions {
  glm::vec3 color{0.7f};
  bool smooth = false;
  bool edges = false;
  float edgeWidth = 1.0f;
  glm::vec3 edgeColor{0.0f};
};

class SurfaceMesh : public Structure {
public:
  using Triangle = std::array<std::uint32_t, 3>;

  SurfaceMesh(std::string name, std::vector<glm::vec3> positions, std::vector<Triangle> triangles);

  const std::vector<glm::vec3> &positions() const;
  const std::vector<Triangle> &triangles() const;
  const std::vector<glm::vec3> &normals() const;

  Bounds bounds() const override;

  SurfaceMeshOptions options;

private:
  void computeNormals();

  std::vector<glm::vec3> m_positions;
  std::vector<Triangle> m_triangles;
  std::vector<glm::vec3> m_normals;
  Bounds m_bounds;
};

} // namespace cartan::render
