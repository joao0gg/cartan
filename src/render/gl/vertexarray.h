#pragma once

#include <cstddef>

namespace cartan::render::gl {

class Buffer;

class VertexArray {
public:
  VertexArray();
  ~VertexArray();

  VertexArray(const VertexArray &) = delete;
  VertexArray &operator=(const VertexArray &) = delete;

  void bind() const;
  void unbind() const;

  void setFloatAttribute(unsigned int location, const Buffer &buffer, int components,
                         std::size_t stride, std::size_t offset);

private:
  unsigned int m_id = 0;
};

} // namespace cartan::render::gl
