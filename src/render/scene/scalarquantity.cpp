#include "render/scene/scalarquantity.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace cartan::render {

ScalarQuantity::ScalarQuantity(std::string name, ScalarLocation location, std::vector<float> values)
    : m_name(std::move(name)), m_location(location), m_values(std::move(values)) {
  float min = std::numeric_limits<float>::max();
  float max = std::numeric_limits<float>::lowest();

  for (const float value : m_values) {
    if (std::isfinite(value)) {
      min = std::min(min, value);
      max = std::max(max, value);
    }
  }

  if (min <= max) {
    m_dataRange = {min, max};
  }
}

const std::string &ScalarQuantity::name() const {
  return m_name;
}

ScalarLocation ScalarQuantity::location() const {
  return m_location;
}

const std::vector<float> &ScalarQuantity::values() const {
  return m_values;
}

std::pair<float, float> ScalarQuantity::dataRange() const {
  return m_dataRange;
}

std::pair<float, float> ScalarQuantity::range() const {
  return options.autoRange ? m_dataRange : std::pair{options.min, options.max};
}

} // namespace cartan::render
