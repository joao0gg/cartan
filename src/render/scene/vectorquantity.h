#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>

namespace cartan::render {

enum class VectorLocation {
  Vertex,
  Face,
};

struct VectorOptions {
  bool enabled = true;
  bool autoScale = true;
  float lengthScale = 1.0f;
  float radius = 0.04f;
  glm::vec3 color{0.1f, 0.1f, 0.1f};
};

class VectorQuantity {
public:
  VectorQuantity(std::string name, VectorLocation location, std::vector<glm::vec3> vectors);

  const std::string &name() const;
  VectorLocation location() const;
  const std::vector<glm::vec3> &vectors() const;

  float maxLength() const;

  VectorOptions options;

private:
  std::string m_name;
  VectorLocation m_location;
  std::vector<glm::vec3> m_vectors;
  float m_maxLength = 0.0f;
};

} // namespace cartan::render
