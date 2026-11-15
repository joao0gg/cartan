#pragma once

#include <memory>
#include <string>
#include <vector>

#include "render/scene/bounds.h"
#include "render/scene/structure.h"
#include "render/scene/surfacemesh.h"

namespace cartan::render {

class Scene {
public:
  SurfaceMesh &addSurfaceMesh(const std::string &name, std::vector<glm::vec3> positions,
                              std::vector<SurfaceMesh::Triangle> triangles);

  void remove(const std::string &name);
  void clear();

  Structure *find(const std::string &name) const;
  const std::vector<std::unique_ptr<Structure>> &structures() const;

  Bounds bounds() const;

private:
  std::vector<std::unique_ptr<Structure>> m_structures;
};

} // namespace cartan::render
