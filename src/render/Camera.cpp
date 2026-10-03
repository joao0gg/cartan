#include "render/Camera.h"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace cartan::render {

namespace {

constexpr glm::vec3 kCameraUpAxis(0.0f, 1.0f, 0.0f);
constexpr float kCameraFov = 45.0f;
constexpr float kCameraPitchLimit = 89.0f;
constexpr float kCameraPanSpeed = 0.002f;
constexpr float kCameraZoomPerStep = 0.9f;
constexpr float kCameraMinDistance = 1.0e-4f;
constexpr float kCameraMaxDistance = 1.0e6f;
constexpr float kCameraMinAspect = 0.001f;
constexpr float kCameraFrameMargin = 1.2f;

} // namespace

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
  return glm::lookAt(position(), m_target, kCameraUpAxis);
}

glm::mat4 Camera::projection(float aspect) const {
  return glm::perspective(glm::radians(kCameraFov), std::max(aspect, kCameraMinAspect), m_nearPlane,
                          m_farPlane);
}

void Camera::orbit(float dx, float dy) {
  m_yaw -= dx;
  m_pitch = std::clamp(m_pitch + dy, -kCameraPitchLimit, kCameraPitchLimit);
}

void Camera::pan(float dx, float dy) {
  const glm::vec3 forward = glm::normalize(m_target - position());
  const glm::vec3 right = glm::normalize(glm::cross(forward, kCameraUpAxis));
  const glm::vec3 up = glm::cross(right, forward);

  m_target += (up * dy - right * dx) * (m_distance * kCameraPanSpeed);
}

void Camera::zoom(float steps) {
  // clang-format off
  m_distance = std::clamp(m_distance * std::pow(kCameraZoomPerStep, steps), 
                          kCameraMinDistance,
                          kCameraMaxDistance);
  // clang-format on
}

void Camera::frame(const glm::vec3 &boundsMin, const glm::vec3 &boundsMax) {
  m_target = 0.5f * (boundsMin + boundsMax);

  const float radius = 0.5f * glm::length(boundsMax - boundsMin);

  m_distance = radius > 0.0f
                   ? radius / std::sin(glm::radians(kCameraFov * 0.5f)) * kCameraFrameMargin
                   : kCameraDefaultDistance;

  m_nearPlane = m_distance * kCameraNearRatio;
  m_farPlane = m_distance * kCameraFarRatio;
}

} // namespace cartan::render