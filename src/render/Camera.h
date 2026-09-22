#pragma once

#include <glm/glm.hpp>

#define CAMERA_UP_AXIS glm::vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_FOV 45.0f
#define CAMERA_DEFAULT_DISTANCE 5.0f
#define CAMERA_DEFAULT_YAW 45.0f
#define CAMERA_DEFAULT_PITCH 25.0f
#define CAMERA_PITCH_LIMIT 89.0f
#define CAMERA_PAN_SPEED 0.002f
#define CAMERA_ZOOM_PER_STEP 0.9f
#define CAMERA_MIN_DISTANCE 1.0e-4f
#define CAMERA_MAX_DISTANCE 1.0e6f
#define CAMERA_MIN_ASPECT 0.001f
#define CAMERA_FRAME_MARGIN 1.2f
#define CAMERA_NEAR_RATIO 0.001f
#define CAMERA_FAR_RATIO 100.0f

namespace cartan::render {

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
  float m_distance = CAMERA_DEFAULT_DISTANCE;
  float m_yaw = CAMERA_DEFAULT_YAW;
  float m_pitch = CAMERA_DEFAULT_PITCH;
  float m_nearPlane = CAMERA_DEFAULT_DISTANCE * CAMERA_NEAR_RATIO;
  float m_farPlane = CAMERA_DEFAULT_DISTANCE * CAMERA_FAR_RATIO;
};

} // namespace cartan::render