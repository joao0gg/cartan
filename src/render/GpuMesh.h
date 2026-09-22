#pragma once

#include "render/GLFunctions.h"
#include "render/RenderMesh.h"

namespace cartan::render {

class GpuMesh {
public:
  void upload(GL &gl, const RenderMesh &mesh);
  void destroy(GL &gl);
  void draw(GL &gl) const;

  bool empty() const {
    return m_indexCount == 0;
  }

private:
  unsigned int m_vao = 0;
  unsigned int m_vbo = 0;
  unsigned int m_ebo = 0;
  int m_indexCount = 0;
};

} // namespace cartan::render