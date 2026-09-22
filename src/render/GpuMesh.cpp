#include "render/GpuMesh.h"

#include <cstddef>

namespace cartan::render {

void GpuMesh::upload(GL &gl, const RenderMesh &mesh) {
  destroy(gl);
  if (mesh.empty()) {
    return;
  }

  gl.glGenVertexArrays(1, &m_vao);
  gl.glGenBuffers(1, &m_vbo);
  gl.glGenBuffers(1, &m_ebo);

  gl.glBindVertexArray(m_vao);

  gl.glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  gl.glBufferData(GL_ARRAY_BUFFER,
                  static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(RenderMesh::Vertex)),
                  mesh.vertices.data(), GL_STATIC_DRAW);

  gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                  static_cast<GLsizeiptr>(mesh.indices.size() * sizeof(std::uint32_t)),
                  mesh.indices.data(), GL_STATIC_DRAW);

  constexpr auto stride = static_cast<GLsizei>(sizeof(RenderMesh::Vertex));

  gl.glEnableVertexAttribArray(0);
  gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                           reinterpret_cast<void *>(offsetof(RenderMesh::Vertex, position)));

  gl.glEnableVertexAttribArray(1);
  gl.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                           reinterpret_cast<void *>(offsetof(RenderMesh::Vertex, normal)));

  gl.glBindVertexArray(0);
  m_indexCount = static_cast<int>(mesh.indices.size());
}

void GpuMesh::destroy(GL &gl) {
  if (m_ebo != 0) {
    gl.glDeleteBuffers(1, &m_ebo);
  }

  if (m_vbo != 0) {
    gl.glDeleteBuffers(1, &m_vbo);
  }

  if (m_vao != 0) {
    gl.glDeleteVertexArrays(1, &m_vao);
  }

  m_vao = m_vbo = m_ebo = 0;
  m_indexCount = 0;
}

void GpuMesh::draw(GL &gl) const {
  if (m_indexCount == 0) {
    return;
  }

  gl.glBindVertexArray(m_vao);
  gl.glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
  gl.glBindVertexArray(0);
}

} // namespace cartan::render