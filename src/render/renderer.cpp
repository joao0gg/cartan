#include "render/renderer.h"

#include <glad/gl.h>

namespace cartan::render {

namespace {

constexpr int kRequiredMajor = 4;
constexpr int kRequiredMinor = 1;

std::string glString(GLenum name) {
  const auto *value = reinterpret_cast<const char *>(glGetString(name));

  return value != nullptr ? value : "";
}

} // namespace

bool Renderer::initialize(GLLoader loader) {
  const int version = gladLoadGL(loader);

  if (version == 0) {
    return false;
  }

  m_info.vendor = glString(GL_VENDOR);
  m_info.renderer = glString(GL_RENDERER);
  m_info.version = glString(GL_VERSION);
  m_info.glsl = glString(GL_SHADING_LANGUAGE_VERSION);

  const int major = GLAD_VERSION_MAJOR(version);
  const int minor = GLAD_VERSION_MINOR(version);

  return major > kRequiredMajor || (major == kRequiredMajor && minor >= kRequiredMinor);
}

const GLInfo &Renderer::info() const {
  return m_info;
}

void Renderer::resize(int width, int height) {
  m_width = width;
  m_height = height;
}

void Renderer::render(unsigned int framebuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, m_width, m_height);

  glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace cartan::render
