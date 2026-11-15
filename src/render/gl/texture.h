#pragma once

#include <vector>

#include <glm/vec3.hpp>

namespace cartan::render::gl {

class Texture1D {
public:
  explicit Texture1D(const std::vector<glm::vec3> &colors);
  ~Texture1D();

  Texture1D(const Texture1D &) = delete;
  Texture1D &operator=(const Texture1D &) = delete;

  void bind(unsigned int unit) const;

private:
  unsigned int m_id = 0;
};

} // namespace cartan::render::gl
