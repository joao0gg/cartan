#pragma once

#include <cstddef>

namespace cartan::render::gl {

class Buffer {
public:
  Buffer();
  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  void bind() const;
  void upload(const void *data, std::size_t size);

private:
  unsigned int m_id = 0;
};

} // namespace cartan::render::gl
