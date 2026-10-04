// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "app/Scene.h"

namespace cartan::app {

void Scene::add(std::string name, core::Mesh mesh) {
  m_objects.push_back({std::move(name), std::move(mesh)});
  notify();
}

void Scene::clear() {
  m_objects.clear();
  notify();
}

Scene::ListenerId Scene::subscribe(std::function<void()> listener) {
  const ListenerId id = m_nextListenerId++;
  m_listeners.emplace_back(id, std::move(listener));

  return id;
}

void Scene::unsubscribe(ListenerId id) {
  std::erase_if(m_listeners, [id](const auto &entry) {
    return entry.first == id;
  });
}

void Scene::notify() const {
  const auto listeners = m_listeners;

  for (const auto &[id, listener] : listeners) {
    listener();
  }
}

} // namespace cartan::app
