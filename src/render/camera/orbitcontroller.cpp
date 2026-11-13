#include "render/camera/orbitcontroller.h"

#include <algorithm>
#include <cmath>

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

namespace cartan::render {

namespace {

constexpr float kOrbitSpeed = 0.01f;
constexpr float kZoomFactor = 0.9f;
constexpr float kPitchLimit = glm::half_pi<float>() - 0.01f;
constexpr float kInitialYaw = glm::quarter_pi<float>();
constexpr float kInitialPitch = 0.5f;
constexpr float kNearRatio = 0.01f;
constexpr float kFarRatio = 100.0f;

} // namespace

OrbitController::OrbitController(Camera &camera)
    : m_camera(camera), m_yaw(kInitialYaw), m_pitch(kInitialPitch) {
  fit(glm::vec3(0.0f), 1.0f);
}

void OrbitController::orbit(float dx, float dy) {
  m_yaw -= dx * kOrbitSpeed;
  m_pitch = std::clamp(m_pitch + dy * kOrbitSpeed, -kPitchLimit, kPitchLimit);

  apply();
}

void OrbitController::pan(float dx, float dy, float viewportHeight) {
  const float scale = 2.0f * m_distance * std::tan(m_camera.fov / 2.0f) / viewportHeight;

  const glm::vec3 forward = glm::normalize(m_camera.target - m_camera.position);
  const glm::vec3 right = glm::normalize(glm::cross(forward, m_camera.up));
  const glm::vec3 up = glm::cross(right, forward);

  m_target += (-dx * right + dy * up) * scale;

  apply();
}

void OrbitController::zoom(float steps) {
  m_distance *= std::pow(kZoomFactor, steps);

  apply();
}

void OrbitController::fit(const glm::vec3 &center, float radius) {
  m_target = center;
  m_distance = radius / std::sin(m_camera.fov / 2.0f);

  apply();
}

void OrbitController::apply() {
  const glm::vec3 direction{
      std::cos(m_pitch) * std::sin(m_yaw),
      std::sin(m_pitch),
      std::cos(m_pitch) * std::cos(m_yaw),
  };

  m_camera.target = m_target;
  m_camera.position = m_target + m_distance * direction;
  m_camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
  m_camera.nearPlane = m_distance * kNearRatio;
  m_camera.farPlane = m_distance * kFarRatio;
}

} // namespace cartan::render
