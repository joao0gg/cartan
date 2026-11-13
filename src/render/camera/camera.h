#pragma once

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

namespace cartan::render {

class Camera {
public:
  glm::vec3 position{0.0f, 0.0f, 1.0f};
  glm::vec3 target{0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  float fov = glm::radians(45.0f);
  float nearPlane = 0.01f;
  float farPlane = 100.0f;
  float aspect = 1.0f;

  glm::mat4 view() const;
  glm::mat4 projection() const;
};

} // namespace cartan::render
