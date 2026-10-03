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
