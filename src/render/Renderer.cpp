#include "render/Renderer.h"

#include <QFile>

#include <utility>

#include <glm/gtc/matrix_inverse.hpp>

#include "render/RenderMesh.h"

namespace cartan::render {

namespace {

constexpr glm::vec4 kRendererClearColor(0.13f, 0.14f, 0.16f, 1.0f);
constexpr glm::vec3 kRendererSurfaceColor(0.72f, 0.74f, 0.78f);

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

std::optional<Renderer> Renderer::create(GL &gl, std::string &error) {
  std::string vertex;
  std::string fragment;

  if (!readResource(":/render/shaders/surface.vert", vertex, error) ||
      !readResource(":/render/shaders/surface.frag", fragment, error)) {
    return std::nullopt;
  }

  std::optional<Shader> surface = Shader::build(gl, vertex.c_str(), fragment.c_str(), error);

  if (!surface) {
    return std::nullopt;
  }

  return Renderer(gl, std::move(*surface));
}

Renderer::Renderer(GL &gl, Shader surface) : m_gl(&gl), m_surface(std::move(surface)) {
}

void Renderer::addMesh(const core::Mesh &mesh) {
  m_meshes.emplace_back(*m_gl, RenderMesh(mesh));
}

void Renderer::clearMeshes() {
  m_meshes.clear();
}

void Renderer::draw(const Camera &camera, int width, int height) {
  m_gl->glViewport(0, 0, width, height);
  m_gl->glEnable(GL_DEPTH_TEST);
  m_gl->glClearColor(kRendererClearColor.r, kRendererClearColor.g, kRendererClearColor.b,
                     kRendererClearColor.a);
  m_gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_meshes.empty()) {
    return;
  }

  const float aspect = static_cast<float>(width) / static_cast<float>(height);
  const glm::mat4 modelView = camera.view();

  m_surface.bind();
  m_surface.setMat4("uModelView", modelView);
  m_surface.setMat4("uProjection", camera.projection(aspect));
  m_surface.setMat3("uNormalMatrix", glm::inverseTranspose(glm::mat3(modelView)));
  m_surface.setVec3("uColor", kRendererSurfaceColor);

  for (const auto &mesh : m_meshes) {
    mesh.draw();
  }
}

} // namespace cartan::render
