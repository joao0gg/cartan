// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "core/Mesh.h"

namespace cartan::app {

struct SceneObject {
  std::string name;
  core::Mesh mesh;
};

class Scene {
public:
  using ListenerId = std::uint64_t;

  Scene() = default;

  Scene(const Scene &) = delete;
  Scene &operator=(const Scene &) = delete;

  void add(std::string name, core::Mesh mesh);
  void clear();

  const std::vector<SceneObject> &objects() const {
    return m_objects;
  }

  ListenerId subscribe(std::function<void()> listener);
  void unsubscribe(ListenerId id);

private:
  void notify() const;

  std::vector<SceneObject> m_objects;
  std::vector<std::pair<ListenerId, std::function<void()>>> m_listeners;
  ListenerId m_nextListenerId = 1;
};

} // namespace cartan::app
