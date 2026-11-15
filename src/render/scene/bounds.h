#pragma once

#include <limits>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace cartan::render {

struct Bounds {
  glm::vec3 min{std::numeric_limits<float>::max()};
  glm::vec3 max{std::numeric_limits<float>::lowest()};

  bool empty() const {
    return min.x > max.x;
  }

  void extend(const glm::vec3 &point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
  }

  void extend(const Bounds &other) {
    if (!other.empty()) {
      extend(other.min);
      extend(other.max);
    }
  }

  glm::vec3 center() const {
    return (min + max) / 2.0f;
  }

  float radius() const {
    return glm::length(max - min) / 2.0f;
  }
};

} // namespace cartan::render
