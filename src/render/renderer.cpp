#include "render/renderer.h"

#include <stdexcept>

#include <glad/gl.h>

#include "render/grid.h"

namespace cartan::render {

namespace {

constexpr int kRequiredMajor = 4;
constexpr int kRequiredMinor = 1;

std::string glString(GLenum name) {
  const auto *value = reinterpret_cast<const char *>(glGetString(name));

  return value != nullptr ? value : "";
}

} // namespace

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

void Renderer::initialize(GLLoader loader) {
  const int version = gladLoadGL(loader);

  if (version == 0) {
    throw std::runtime_error("Failed to load OpenGL");
  }

  m_info.vendor = glString(GL_VENDOR);
  m_info.renderer = glString(GL_RENDERER);
  m_info.version = glString(GL_VERSION);
  m_info.glsl = glString(GL_SHADING_LANGUAGE_VERSION);

  const int major = GLAD_VERSION_MAJOR(version);
  const int minor = GLAD_VERSION_MINOR(version);

  if (major < kRequiredMajor || (major == kRequiredMajor && minor < kRequiredMinor)) {
    throw std::runtime_error("OpenGL 4.1 core is not available (got " + m_info.version + ")");
  }

  glEnable(GL_DEPTH_TEST);

  m_grid = std::make_unique<Grid>();
}

void Renderer::release() {
  m_grid.reset();
}

const GLInfo &Renderer::info() const {
  return m_info;
}

void Renderer::resize(int width, int height) {
  m_width = width;
  m_height = height;
}

void Renderer::render(unsigned int framebuffer, const Camera &camera) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, m_width, m_height);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_grid) {
    m_grid->draw(camera);
  }
}

} // namespace cartan::render
