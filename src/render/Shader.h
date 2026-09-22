#pragma once

#include <string>

#include <glm/glm.hpp>

#include "render/GLFunctions.h"

namespace cartan::render {

class Shader {
public:
  bool build(GL &gl, const char *vertexPath, const char *fragmentPath, std::string &error);
  void destroy(GL &gl);

  void bind(GL &gl) const;
  void setMat4(GL &gl, const char *name, const glm::mat4 &value) const;
  void setMat3(GL &gl, const char *name, const glm::mat3 &value) const;
  void setVec3(GL &gl, const char *name, const glm::vec3 &value) const;

private:
  unsigned int m_program = 0;
};

} // namespace cartan::render