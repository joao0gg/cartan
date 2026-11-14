#pragma once

#include <glm/vec3.hpp>

namespace cartan::render {

struct GridStyle {
  glm::vec3 minorColor{0.0f};
  glm::vec3 majorColor{0.0f};
  glm::vec3 xAxisColor{1.0f, 0.0f, 0.0f};
  glm::vec3 zAxisColor{0.0f, 0.0f, 1.0f};
  float minorAlpha = 0.1f;
  float majorAlpha = 0.25f;
  float cellSize = 1.0f;
};

struct Theme {
  glm::vec3 background{1.0f};
  GridStyle grid;
};

} // namespace cartan::render
