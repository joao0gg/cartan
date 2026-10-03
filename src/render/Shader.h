#pragma once

#include <optional>
#include <string>

#include <glm/glm.hpp>

#include "render/GLFunctions.h"

namespace cartan::render {

class Shader {
public:
  static std::optional<Shader> build(GL &gl, const char *vertexSource, const char *fragmentSource,
                                     std::string &error);

  ~Shader();

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  void bind() const;
  void setMat4(const char *name, const glm::mat4 &value) const;
  void setMat3(const char *name, const glm::mat3 &value) const;
  void setVec3(const char *name, const glm::vec3 &value) const;

private:
  Shader(GL &gl, unsigned int program);

  void release();

  GL *m_gl = nullptr;
  unsigned int m_program = 0;
};

} // namespace cartan::render
