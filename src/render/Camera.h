// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <glm/glm.hpp>

namespace cartan::render {

constexpr float kCameraDefaultDistance = 5.0f;
constexpr float kCameraDefaultYaw = 45.0f;
constexpr float kCameraDefaultPitch = 25.0f;
constexpr float kCameraNearRatio = 0.001f;
constexpr float kCameraFarRatio = 100.0f;

class Camera {
public:
  glm::vec3 position() const;
  glm::mat4 view() const;
  glm::mat4 projection(float aspect) const;

  void orbit(float dx, float dy);
  void pan(float dx, float dy);
  void zoom(float steps);
  void frame(const glm::vec3 &boundsMin, const glm::vec3 &boundsMax);

private:
  glm::vec3 m_target{0.0f};
  float m_distance = kCameraDefaultDistance;
  float m_yaw = kCameraDefaultYaw;
  float m_pitch = kCameraDefaultPitch;
  float m_nearPlane = kCameraDefaultDistance * kCameraNearRatio;
  float m_farPlane = kCameraDefaultDistance * kCameraFarRatio;
};

} // namespace cartan::render