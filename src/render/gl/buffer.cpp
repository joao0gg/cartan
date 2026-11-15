#include "render/gl/buffer.h"

#include <glad/gl.h>

namespace cartan::render::gl {

Buffer::Buffer() {
  glGenBuffers(1, &m_id);
}

Buffer::~Buffer() {
  glDeleteBuffers(1, &m_id);
}

void Buffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void Buffer::upload(const void *data, std::size_t size) {
  bind();
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
}

} // namespace cartan::render::gl
