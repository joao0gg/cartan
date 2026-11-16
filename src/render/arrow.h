#pragma once

#include "render/gl/buffer.h"

namespace cartan::render {

namespace gl {
class VertexArray;
}

class ArrowModel {
public:
  ArrowModel();

  void attach(gl::VertexArray &vertexArray) const;
  int count() const;

private:
  gl::Buffer m_buffer;
  int m_count = 0;
};

} // namespace cartan::render
