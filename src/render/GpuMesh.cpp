// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "render/GpuMesh.h"

#include <cstddef>
#include <utility>

namespace cartan::render {

GpuMesh::GpuMesh(GL &gl, const RenderMesh &mesh) : m_gl(&gl) {
  if (mesh.indices.empty()) {
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

GpuMesh::~GpuMesh() {
  release();
}

GpuMesh::GpuMesh(GpuMesh &&other) noexcept
    : m_gl(std::exchange(other.m_gl, nullptr)), m_vao(std::exchange(other.m_vao, 0)),
      m_vbo(std::exchange(other.m_vbo, 0)), m_ebo(std::exchange(other.m_ebo, 0)),
      m_indexCount(std::exchange(other.m_indexCount, 0)) {
}

GpuMesh &GpuMesh::operator=(GpuMesh &&other) noexcept {
  if (this != &other) {
    release();

    m_gl = std::exchange(other.m_gl, nullptr);
    m_vao = std::exchange(other.m_vao, 0);
    m_vbo = std::exchange(other.m_vbo, 0);
    m_ebo = std::exchange(other.m_ebo, 0);
    m_indexCount = std::exchange(other.m_indexCount, 0);
  }

  return *this;
}

void GpuMesh::release() {
  if (m_gl == nullptr) {
    return;
  }

  if (m_ebo != 0) {
    m_gl->glDeleteBuffers(1, &m_ebo);
  }

  if (m_vbo != 0) {
    m_gl->glDeleteBuffers(1, &m_vbo);
  }

  if (m_vao != 0) {
    m_gl->glDeleteVertexArrays(1, &m_vao);
  }

  m_vao = m_vbo = m_ebo = 0;
  m_indexCount = 0;
}

void GpuMesh::draw() const {
  if (m_indexCount == 0) {
    return;
  }

  m_gl->glBindVertexArray(m_vao);
  m_gl->glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
  m_gl->glBindVertexArray(0);
}

} // namespace cartan::render
