#pragma once

#include <string>
#include <vector>

#include "core/Mesh.h"
#include "render/Camera.h"
#include "render/GLFunctions.h"
#include "render/GpuMesh.h"
#include "render/Shader.h"

#define RENDERER_CLEAR_COLOR 0.13f, 0.14f, 0.16f, 1.0f
#define RENDERER_SURFACE_COLOR glm::vec3(0.72f, 0.74f, 0.78f)

namespace cartan::render {

class Renderer {
public:
  bool initialize(GL &gl, std::string &error);
  void shutdown(GL &gl);

  void addMesh(GL &gl, const core::Mesh &mesh);
  void clearMeshes(GL &gl);

  void draw(GL &gl, const Camera &camera, int width, int height);

private:
  Shader m_surface;
  std::vector<GpuMesh> m_meshes;
};

} // namespace cartan::render
