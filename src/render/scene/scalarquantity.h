#pragma once

#include <string>
#include <utility>
#include <vector>

#include <glm/vec3.hpp>

#include "render/colormap.h"

namespace cartan::render {

enum class ScalarLocation {
  Vertex,
  Face,
};

struct ScalarOptions {
  Colormap colormap = Colormap::Rainbow;
  bool autoRange = true;
  float min = 0.0f;
  float max = 1.0f;
  glm::vec3 invalidColor{0.5f};
};

class ScalarQuantity {
public:
  ScalarQuantity(std::string name, ScalarLocation location, std::vector<float> values);

  const std::string &name() const;
  ScalarLocation location() const;
  const std::vector<float> &values() const;

  std::pair<float, float> dataRange() const;
  std::pair<float, float> range() const;

  ScalarOptions options;

private:
  std::string m_name;
  ScalarLocation m_location;
  std::vector<float> m_values;
  std::pair<float, float> m_dataRange{0.0f, 0.0f};
};

} // namespace cartan::render
