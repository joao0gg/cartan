#include "render/scene/scene.h"

#include <algorithm>
#include <utility>

namespace cartan::render {

SurfaceMesh &Scene::addSurfaceMesh(const std::string &name, std::vector<glm::vec3> positions,
                                   std::vector<SurfaceMesh::Triangle> triangles) {
  auto mesh = std::make_unique<SurfaceMesh>(name, std::move(positions), std::move(triangles));
  SurfaceMesh &result = *mesh;

  remove(name);
  m_structures.push_back(std::move(mesh));

  return result;
}

void Scene::remove(const std::string &name) {
  std::erase_if(m_structures, [&name](const std::unique_ptr<Structure> &structure) {
    return structure->name() == name;
  });
}

void Scene::clear() {
  m_structures.clear();
}

Structure *Scene::find(const std::string &name) const {
  for (const auto &structure : m_structures) {
    if (structure->name() == name) {
      return structure.get();
    }
  }

  return nullptr;
}

const std::vector<std::unique_ptr<Structure>> &Scene::structures() const {
  return m_structures;
}

Bounds Scene::bounds() const {
  Bounds bounds;

  for (const auto &structure : m_structures) {
    if (structure->visible) {
      bounds.extend(structure->bounds());
    }
  }

  return bounds;
}

} // namespace cartan::render
