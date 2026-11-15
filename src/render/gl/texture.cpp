#include "render/gl/texture.h"

#include <glad/gl.h>

namespace cartan::render::gl {

Texture1D::Texture1D(const std::vector<glm::vec3> &colors) {
  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_1D, m_id);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, static_cast<GLsizei>(colors.size()), 0, GL_RGB,
               GL_FLOAT, colors.data());

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_1D, 0);
}

Texture1D::~Texture1D() {
  glDeleteTextures(1, &m_id);
}

void Texture1D::bind(unsigned int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_1D, m_id);
}

} // namespace cartan::render::gl
