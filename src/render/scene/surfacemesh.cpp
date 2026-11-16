#include "render/scene/surfacemesh.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include <glm/geometric.hpp>

namespace cartan::render {

SurfaceMesh::SurfaceMesh(std::string name, std::vector<glm::vec3> positions,
                         std::vector<Triangle> triangles)
    : Structure(std::move(name)), m_positions(std::move(positions)),
      m_triangles(std::move(triangles)) {
  for (const Triangle &triangle : m_triangles) {
    for (const std::uint32_t index : triangle) {
      if (index >= m_positions.size()) {
        throw std::invalid_argument("triangle index out of range");
      }
    }
  }

  for (const glm::vec3 &position : m_positions) {
    m_bounds.extend(position);
  }

  computeNormals();
}

const std::vector<glm::vec3> &SurfaceMesh::positions() const {
  return m_positions;
}

const std::vector<SurfaceMesh::Triangle> &SurfaceMesh::triangles() const {
  return m_triangles;
}

const std::vector<glm::vec3> &SurfaceMesh::normals() const {
  return m_normals;
}

Bounds SurfaceMesh::bounds() const {
  return m_bounds;
}

ScalarQuantity &SurfaceMesh::addVertexScalar(const std::string &name, std::vector<float> values) {
  if (values.size() != m_positions.size()) {
    throw std::invalid_argument("vertex scalar \"" + name + "\" needs one value per vertex");
  }

  return addScalar(name, ScalarLocation::Vertex, std::move(values));
}

ScalarQuantity &SurfaceMesh::addFaceScalar(const std::string &name, std::vector<float> values) {
  if (values.size() != m_triangles.size()) {
    throw std::invalid_argument("face scalar \"" + name + "\" needs one value per triangle");
  }

  return addScalar(name, ScalarLocation::Face, std::move(values));
}

ScalarQuantity &SurfaceMesh::addScalar(const std::string &name, ScalarLocation location,
                                       std::vector<float> values) {
  const bool wasActive = m_active != nullptr && m_active->name() == name;

  removeQuantity(name);

  auto quantity = std::make_unique<ScalarQuantity>(name, location, std::move(values));
  ScalarQuantity &result = *quantity;
  m_quantities.push_back(std::move(quantity));

  if (wasActive) {
    m_active = &result;
  }

  ++m_version;

  return result;
}

void SurfaceMesh::removeQuantity(const std::string &name) {
  if (m_active != nullptr && m_active->name() == name) {
    m_active = nullptr;
  }

  std::erase_if(m_quantities, [&name](const std::unique_ptr<ScalarQuantity> &quantity) {
    return quantity->name() == name;
  });

  std::erase_if(m_vectorQuantities, [&name](const std::unique_ptr<VectorQuantity> &quantity) {
    return quantity->name() == name;
  });

  ++m_version;
}

const std::vector<std::unique_ptr<ScalarQuantity>> &SurfaceMesh::quantities() const {
  return m_quantities;
}

ScalarQuantity *SurfaceMesh::findQuantity(const std::string &name) const {
  for (const auto &quantity : m_quantities) {
    if (quantity->name() == name) {
      return quantity.get();
    }
  }

  return nullptr;
}

VectorQuantity &SurfaceMesh::addVertexVector(const std::string &name,
                                             std::vector<glm::vec3> vectors) {
  if (vectors.size() != m_positions.size()) {
    throw std::invalid_argument("vertex vector \"" + name + "\" needs one vector per vertex");
  }

  return addVector(name, VectorLocation::Vertex, std::move(vectors));
}

VectorQuantity &SurfaceMesh::addFaceVector(const std::string &name,
                                           std::vector<glm::vec3> vectors) {
  if (vectors.size() != m_triangles.size()) {
    throw std::invalid_argument("face vector \"" + name + "\" needs one vector per triangle");
  }

  return addVector(name, VectorLocation::Face, std::move(vectors));
}

VectorQuantity &SurfaceMesh::addVector(const std::string &name, VectorLocation location,
                                       std::vector<glm::vec3> vectors) {
  removeQuantity(name);

  auto quantity = std::make_unique<VectorQuantity>(name, location, std::move(vectors));
  VectorQuantity &result = *quantity;
  m_vectorQuantities.push_back(std::move(quantity));

  ++m_version;

  return result;
}

const std::vector<std::unique_ptr<VectorQuantity>> &SurfaceMesh::vectorQuantities() const {
  return m_vectorQuantities;
}

VectorQuantity *SurfaceMesh::findVectorQuantity(const std::string &name) const {
  for (const auto &quantity : m_vectorQuantities) {
    if (quantity->name() == name) {
      return quantity.get();
    }
  }

  return nullptr;
}

void SurfaceMesh::setActiveQuantity(const std::string &name) {
  m_active = name.empty() ? nullptr : findQuantity(name);
  ++m_version;
}

ScalarQuantity *SurfaceMesh::activeQuantity() const {
  return m_active;
}

std::uint64_t SurfaceMesh::version() const {
  return m_version;
}

void SurfaceMesh::computeNormals() {
  m_normals.assign(m_positions.size(), glm::vec3(0.0f));

  for (const Triangle &triangle : m_triangles) {
    const glm::vec3 &a = m_positions[triangle[0]];
    const glm::vec3 &b = m_positions[triangle[1]];
    const glm::vec3 &c = m_positions[triangle[2]];

    const glm::vec3 normal = glm::cross(b - a, c - a);

    for (const std::uint32_t index : triangle) {
      m_normals[index] += normal;
    }
  }

  for (glm::vec3 &normal : m_normals) {
    const float length = glm::length(normal);
    normal = length > 0.0f ? normal / length : glm::vec3(0.0f, 1.0f, 0.0f);
  }
}

} // namespace cartan::render
