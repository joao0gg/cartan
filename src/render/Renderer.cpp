#include "render/Renderer.h"

#include <glm/gtc/matrix_inverse.hpp>

#include "io/ShaderIO.h"

namespace cartan::render {

bool Renderer::initialize(GL &gl, const std::filesystem::path &shaderDir, std::string &error) {
  const std::string vertex = io::readTextFile(shaderDir / "surface.vert");
  const std::string fragment = io::readTextFile(shaderDir / "surface.frag");

  return m_surface.build(gl, vertex.c_str(), fragment.c_str(), error);
}

void Renderer::shutdown(GL &gl) {
  m_mesh.destroy(gl);
  m_surface.destroy(gl);
}

void Renderer::setMesh(GL &gl, const RenderMesh &mesh) {
  m_mesh.upload(gl, mesh);
}

void Renderer::draw(GL &gl, const Camera &camera, int width, int height) {
  gl.glViewport(0, 0, width, height);
  gl.glEnable(GL_DEPTH_TEST);
  gl.glClearColor(RENDERER_CLEAR_COLOR);
  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_mesh.empty()) {
    return;
  }

  const float aspect = static_cast<float>(width) / static_cast<float>(height);
  const glm::mat4 modelView = camera.view();

  m_surface.bind(gl);
  m_surface.setMat4(gl, "uModelView", modelView);
  m_surface.setMat4(gl, "uProjection", camera.projection(aspect));
  m_surface.setMat3(gl, "uNormalMatrix", glm::inverseTranspose(glm::mat3(modelView)));
  m_surface.setVec3(gl, "uColor", RENDERER_SURFACE_COLOR);

  m_mesh.draw(gl);
}

} // namespace cartan::render