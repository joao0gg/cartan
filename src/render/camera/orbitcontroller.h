#pragma once

#include <glm/vec3.hpp>

#include "render/camera/camera.h"

namespace cartan::render {

class OrbitController {
public:
  explicit OrbitController(Camera &camera);

  void orbit(float dx, float dy);
  void pan(float dx, float dy, float viewportHeight);
  void zoom(float steps);
  void fit(const glm::vec3 &center, float radius);

private:
  void apply();

  Camera &m_camera;
  glm::vec3 m_target{0.0f};
  float m_distance = 1.0f;
  float m_yaw = 0.0f;
  float m_pitch = 0.0f;
};

} // namespace cartan::render
