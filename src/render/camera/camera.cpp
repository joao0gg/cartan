#include "render/camera/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace cartan::render {

glm::mat4 Camera::view() const {
  return glm::lookAt(position, target, up);
}

glm::mat4 Camera::projection() const {
  return glm::perspective(fov, aspect, nearPlane, farPlane);
}

} // namespace cartan::render
