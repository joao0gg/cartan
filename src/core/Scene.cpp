#include "core/Scene.h"

#include <utility>

namespace cartan::core {

void Scene::add(std::string name, Mesh mesh) {
  m_objects.push_back({std::move(name), std::move(mesh)});
  emit changed();
}

void Scene::clear() {
  m_objects.clear();
  emit changed();
}

} // namespace cartan::core
