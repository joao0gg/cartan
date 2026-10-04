// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "core/Mesh.h"
#include "render/Camera.h"
#include "render/GLFunctions.h"
#include "render/GpuMesh.h"
#include "render/Shader.h"

namespace cartan::render {

class Renderer {
public:
  static std::optional<Renderer> create(GL &gl, std::string &error);

  void addMesh(const core::Mesh &mesh);
  void clearMeshes();

  void draw(const Camera &camera, int width, int height);

private:
  Renderer(GL &gl, Shader surface);

  GL *m_gl = nullptr;
  Shader m_surface;
  std::vector<GpuMesh> m_meshes;
};

} // namespace cartan::render
