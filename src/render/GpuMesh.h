#pragma once

#include "render/GLFunctions.h"
#include "render/RenderMesh.h"

namespace cartan::render {

class GpuMesh {
public:
  GpuMesh(GL &gl, const RenderMesh &mesh);
  ~GpuMesh();

  GpuMesh(const GpuMesh &) = delete;
  GpuMesh &operator=(const GpuMesh &) = delete;

  GpuMesh(GpuMesh &&other) noexcept;
  GpuMesh &operator=(GpuMesh &&other) noexcept;

  void draw() const;

  bool empty() const {
    return m_indexCount == 0;
  }

private:
  void release();

  GL *m_gl = nullptr;
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;
  int m_indexCount = 0;
};

} // namespace cartan::render
