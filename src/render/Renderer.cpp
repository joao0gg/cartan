#include "render/Renderer.h"

#include <QFile>

#include <glm/gtc/matrix_inverse.hpp>

namespace cartan::render {

namespace {

bool readResource(const QString &path, std::string &text, std::string &error) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    error = "cannot open " + path.toStdString();
    return false;
  }

  text = file.readAll().toStdString();

  return true;
}

} // namespace

bool Renderer::initialize(GL &gl, std::string &error) {
  std::string vertex;
  std::string fragment;

  if (!readResource(":/render/shaders/surface.vert", vertex, error) ||
      !readResource(":/render/shaders/surface.frag", fragment, error)) {
    return false;
  }

  return m_surface.build(gl, vertex.c_str(), fragment.c_str(), error);
}

void Renderer::shutdown(GL &gl) {
  clearMeshes(gl);
  m_surface.destroy(gl);
}

void Renderer::addMesh(GL &gl, const core::Mesh &mesh) {
  m_meshes.emplace_back().upload(gl, mesh);
}

void Renderer::clearMeshes(GL &gl) {
  for (auto &mesh : m_meshes) {
    mesh.destroy(gl);
  }

  m_meshes.clear();
}

void Renderer::draw(GL &gl, const Camera &camera, int width, int height) {
  gl.glViewport(0, 0, width, height);
  gl.glEnable(GL_DEPTH_TEST);
  gl.glClearColor(RENDERER_CLEAR_COLOR);
  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_meshes.empty()) {
    return;
  }

  const float aspect = static_cast<float>(width) / static_cast<float>(height);
  const glm::mat4 modelView = camera.view();

  m_surface.bind(gl);
  m_surface.setMat4(gl, "uModelView", modelView);
  m_surface.setMat4(gl, "uProjection", camera.projection(aspect));
  m_surface.setMat3(gl, "uNormalMatrix", glm::inverseTranspose(glm::mat3(modelView)));
  m_surface.setVec3(gl, "uColor", RENDERER_SURFACE_COLOR);

  for (const auto &mesh : m_meshes) {
    mesh.draw(gl);
  }
}

} // namespace cartan::render
