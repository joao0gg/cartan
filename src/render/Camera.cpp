#include "render/Camera.h"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace cartan::render {

glm::vec3 Camera::position() const {
  const float yaw = glm::radians(m_yaw);
  const float pitch = glm::radians(m_pitch);

  const glm::vec3 direction{
      std::cos(pitch) * std::sin(yaw),
      std::sin(pitch),
      std::cos(pitch) * std::cos(yaw),
  };

  return m_target + m_distance * direction;
}

glm::mat4 Camera::view() const {
  return glm::lookAt(position(), m_target, CAMERA_UP_AXIS);
}

glm::mat4 Camera::projection(float aspect) const {
  return glm::perspective(glm::radians(CAMERA_FOV), std::max(aspect, CAMERA_MIN_ASPECT),
                          m_nearPlane, m_farPlane);
}

void Camera::orbit(float dx, float dy) {
  m_yaw -= dx;
  m_pitch = std::clamp(m_pitch + dy, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
}

void Camera::pan(float dx, float dy) {
  const glm::vec3 forward = glm::normalize(m_target - position());
  const glm::vec3 right = glm::normalize(glm::cross(forward, CAMERA_UP_AXIS));
  const glm::vec3 up = glm::cross(right, forward);

  m_target += (up * dy - right * dx) * (m_distance * CAMERA_PAN_SPEED);
}

void Camera::zoom(float steps) {
  // clang-format off
  m_distance = std::clamp(m_distance * std::pow(CAMERA_ZOOM_PER_STEP, steps), 
                          CAMERA_MIN_DISTANCE,
                          CAMERA_MAX_DISTANCE);
  // clang-format on
}

void Camera::frame(const glm::vec3 &boundsMin, const glm::vec3 &boundsMax) {
  m_target = 0.5f * (boundsMin + boundsMax);

  const float radius = 0.5f * glm::length(boundsMax - boundsMin);

  m_distance = radius > 0.0f
                   ? radius / std::sin(glm::radians(CAMERA_FOV * 0.5f)) * CAMERA_FRAME_MARGIN
                   : CAMERA_DEFAULT_DISTANCE;

  m_nearPlane = m_distance * CAMERA_NEAR_RATIO;
  m_farPlane = m_distance * CAMERA_FAR_RATIO;
}

} // namespace cartan::render