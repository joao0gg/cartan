#pragma once

#include <string_view>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace cartan::render::gl {

class Program {
public:
  Program(std::string_view vertex, std::string_view fragment);
  ~Program();

  Program(const Program &) = delete;
  Program &operator=(const Program &) = delete;

  void use() const;

  void set(const char *name, float value) const;
  void set(const char *name, const glm::vec3 &value) const;
  void set(const char *name, const glm::mat4 &value) const;

private:
  unsigned int m_id = 0;
};

} // namespace cartan::render::gl
