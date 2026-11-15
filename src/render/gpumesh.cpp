#include "render/gpumesh.h"

#include <cstddef>
#include <vector>

#include <glad/gl.h>

namespace cartan::render {

namespace {

struct Corner {
  glm::vec3 position;
  glm::vec3 normal;
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
