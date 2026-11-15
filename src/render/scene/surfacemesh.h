#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <glm/vec3.hpp>

#include "render/scene/scalarquantity.h"
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

  ScalarQuantity &addVertexScalar(const std::string &name, std::vector<float> values);
  ScalarQuantity &addFaceScalar(const std::string &name, std::vector<float> values);
  void removeQuantity(const std::string &name);

  const std::vector<std::unique_ptr<ScalarQuantity>> &quantities() const;
  ScalarQuantity *findQuantity(const std::string &name) const;

  void setActiveQuantity(const std::string &name);
  ScalarQuantity *activeQuantity() const;

  std::uint64_t version() const;

  SurfaceMeshOptions options;

private:
  void computeNormals();
  ScalarQuantity &addScalar(const std::string &name, ScalarLocation location,
                            std::vector<float> values);

  std::vector<glm::vec3> m_positions;
  std::vector<Triangle> m_triangles;
  std::vector<glm::vec3> m_normals;
  Bounds m_bounds;

  std::vector<std::unique_ptr<ScalarQuantity>> m_quantities;
  ScalarQuantity *m_active = nullptr;
  std::uint64_t m_version = 0;
};

} // namespace cartan::render
