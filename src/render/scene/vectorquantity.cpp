#include "render/scene/vectorquantity.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include <glm/geometric.hpp>

namespace cartan::render {

VectorQuantity::VectorQuantity(std::string name, VectorLocation location,
                               std::vector<glm::vec3> vectors)
    : m_name(std::move(name)), m_location(location), m_vectors(std::move(vectors)) {
  for (const glm::vec3 &vector : m_vectors) {
    const float length = glm::length(vector);

    if (std::isfinite(length)) {
      m_maxLength = std::max(m_maxLength, length);
    }
  }
}

const std::string &VectorQuantity::name() const {
  return m_name;
}

VectorLocation VectorQuantity::location() const {
  return m_location;
}

const std::vector<glm::vec3> &VectorQuantity::vectors() const {
  return m_vectors;
}

float VectorQuantity::maxLength() const {
  return m_maxLength;
}

} // namespace cartan::render
