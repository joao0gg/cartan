#include "render/gpumesh.h"

#include <cmath>
#include <cstddef>
#include <vector>

#include <glad/gl.h>
#include <glm/geometric.hpp>

#include "render/arrow.h"

namespace cartan::render {

namespace {

struct Corner {
  glm::vec3 position;
  glm::vec3 normal;
};

struct Instance {
  glm::vec3 base;
  glm::vec3 vector;
};

} // namespace

GpuMesh::GpuMesh(const SurfaceMesh &mesh) {
  const auto &positions = mesh.positions();
  const auto &normals = mesh.normals();

  std::vector<Corner> corners;
  corners.reserve(mesh.triangles().size() * 3);

  for (const SurfaceMesh::Triangle &triangle : mesh.triangles()) {
    for (const std::uint32_t index : triangle) {
      corners.push_back({positions[index], normals[index]});
    }
  }

  m_buffer.upload(corners.data(), corners.size() * sizeof(Corner));
  m_vertexArray.setFloatAttribute(0, m_buffer, 3, sizeof(Corner), offsetof(Corner, position));
  m_vertexArray.setFloatAttribute(1, m_buffer, 3, sizeof(Corner), offsetof(Corner, normal));

  m_count = static_cast<int>(corners.size());

  m_vertexArray.setFloatAttribute(2, m_values, 1, sizeof(float), 0);
}

void GpuMesh::update(const SurfaceMesh &mesh, const ArrowModel &arrow) {
  if (m_hasValues && m_version == mesh.version()) {
    return;
  }

  updateValues(mesh);
  updateVectors(mesh, arrow);

  m_version = mesh.version();
  m_hasValues = true;
}

void GpuMesh::updateValues(const SurfaceMesh &mesh) {
  std::vector<float> values(m_count, 0.0f);
  const ScalarQuantity *quantity = mesh.activeQuantity();

  if (quantity != nullptr) {
    const std::vector<float> &source = quantity->values();
    const auto &triangles = mesh.triangles();

    for (std::size_t face = 0; face < triangles.size(); ++face) {
      for (std::size_t corner = 0; corner < 3; ++corner) {
        values[face * 3 + corner] = quantity->location() == ScalarLocation::Vertex
                                        ? source[triangles[face][corner]]
                                        : source[face];
      }
    }
  }

  m_values.upload(values.data(), values.size() * sizeof(float));
}

void GpuMesh::updateVectors(const SurfaceMesh &mesh, const ArrowModel &arrow) {
  m_vectors.clear();

  const auto &positions = mesh.positions();
  const auto &triangles = mesh.triangles();

  for (const auto &quantity : mesh.vectorQuantities()) {
    std::vector<Instance> instances;
    instances.reserve(quantity->vectors().size());

    for (std::size_t i = 0; i < quantity->vectors().size(); ++i) {
      const glm::vec3 &vector = quantity->vectors()[i];
      const float length = glm::length(vector);

      if (!std::isfinite(length) || length <= 0.0f) {
        continue;
      }

      glm::vec3 base;

      if (quantity->location() == VectorLocation::Vertex) {
        base = positions[i];
      } else {
        const SurfaceMesh::Triangle &triangle = triangles[i];
        base = (positions[triangle[0]] + positions[triangle[1]] + positions[triangle[2]]) / 3.0f;
      }

      instances.push_back({base, vector});
    }

    auto gpu = std::make_unique<GpuVectors>();
    gpu->instances.upload(instances.data(), instances.size() * sizeof(Instance));
    gpu->count = static_cast<int>(instances.size());

    arrow.attach(gpu->vertexArray);
    gpu->vertexArray.setFloatAttribute(4, gpu->instances, 3, sizeof(Instance),
                                       offsetof(Instance, base), 1);
    gpu->vertexArray.setFloatAttribute(5, gpu->instances, 3, sizeof(Instance),
                                       offsetof(Instance, vector), 1);

    m_vectors.push_back(std::move(gpu));
  }
}

void GpuMesh::drawVectors(std::size_t index, const ArrowModel &arrow) const {
  const GpuVectors &vectors = *m_vectors.at(index);

  if (vectors.count == 0) {
    return;
  }

  vectors.vertexArray.bind();
  glDrawArraysInstanced(GL_TRIANGLES, 0, arrow.count(), vectors.count);
  vectors.vertexArray.unbind();
}

void GpuMesh::draw(const gl::Program &program, const SurfaceMeshOptions &options) const {
  program.set("u_color", options.color);
  program.set("u_smooth", options.smooth ? 1 : 0);
  program.set("u_edges", options.edges ? 1 : 0);
  program.set("u_edgeWidth", options.edgeWidth);
  program.set("u_edgeColor", options.edgeColor);

  m_vertexArray.bind();
  glDrawArrays(GL_TRIANGLES, 0, m_count);
  m_vertexArray.unbind();
}

} // namespace cartan::render
