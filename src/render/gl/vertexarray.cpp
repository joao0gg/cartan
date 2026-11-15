#include "render/gl/vertexarray.h"

#include <glad/gl.h>

#include "render/gl/buffer.h"

namespace cartan::render::gl {

VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() const {
  glBindVertexArray(m_id);
}

void VertexArray::unbind() const {
  glBindVertexArray(0);
}

void VertexArray::setFloatAttribute(unsigned int location, const Buffer &buffer, int components,
                                    std::size_t stride, std::size_t offset) {
  bind();
  buffer.bind();

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride),
                        reinterpret_cast<const void *>(offset));

  unbind();
}

} // namespace cartan::render::gl
