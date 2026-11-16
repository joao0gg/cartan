#include "render/arrow.h"

#include <cmath>
#include <cstddef>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "render/gl/vertexarray.h"

namespace cartan::render {

namespace {

constexpr int kSegments = 16;
constexpr float kShaft = 0.0f;
constexpr float kHead = 1.0f;
constexpr float kHeadRadius = 2.0f;
constexpr float kConeSlope = 0.5f;

struct Vertex {
  glm::vec2 offset;
  float z;
  float part;
  glm::vec3 normal;
};

glm::vec2 circle(int i) {
  const float angle = glm::two_pi<float>() * float(i) / float(kSegments);
  return {std::cos(angle), std::sin(angle)};
}

std::vector<Vertex> build() {
  std::vector<Vertex> vertices;

  for (int i = 0; i < kSegments; ++i) {
    const glm::vec2 a = circle(i);
    const glm::vec2 b = circle(i + 1);
    const glm::vec2 middle = glm::normalize(a + b);

    const glm::vec3 normalA{a, 0.0f};
    const glm::vec3 normalB{b, 0.0f};

    vertices.push_back({a, 0.0f, kShaft, normalA});
    vertices.push_back({b, 0.0f, kShaft, normalB});
    vertices.push_back({b, 1.0f, kShaft, normalB});
    vertices.push_back({a, 0.0f, kShaft, normalA});
    vertices.push_back({b, 1.0f, kShaft, normalB});
    vertices.push_back({a, 1.0f, kShaft, normalA});

    const glm::vec3 down{0.0f, 0.0f, -1.0f};

    vertices.push_back({glm::vec2(0.0f), 0.0f, kHead, down});
    vertices.push_back({b * kHeadRadius, 0.0f, kHead, down});
    vertices.push_back({a * kHeadRadius, 0.0f, kHead, down});

    vertices.push_back({a * kHeadRadius, 0.0f, kHead, glm::normalize(glm::vec3(a, kConeSlope))});
    vertices.push_back({b * kHeadRadius, 0.0f, kHead, glm::normalize(glm::vec3(b, kConeSlope))});
    vertices.push_back(
        {glm::vec2(0.0f), 1.0f, kHead, glm::normalize(glm::vec3(middle, kConeSlope))});
  }

  return vertices;
}

} // namespace

ArrowModel::ArrowModel() {
  const std::vector<Vertex> vertices = build();

  m_buffer.upload(vertices.data(), vertices.size() * sizeof(Vertex));
  m_count = static_cast<int>(vertices.size());
}

void ArrowModel::attach(gl::VertexArray &vertexArray) const {
  vertexArray.setFloatAttribute(0, m_buffer, 2, sizeof(Vertex), offsetof(Vertex, offset));
  vertexArray.setFloatAttribute(1, m_buffer, 1, sizeof(Vertex), offsetof(Vertex, z));
  vertexArray.setFloatAttribute(2, m_buffer, 1, sizeof(Vertex), offsetof(Vertex, part));
  vertexArray.setFloatAttribute(3, m_buffer, 3, sizeof(Vertex), offsetof(Vertex, normal));
}

int ArrowModel::count() const {
  return m_count;
}

} // namespace cartan::render
