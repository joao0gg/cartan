#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "render/gl/buffer.h"
#include "render/gl/program.h"
#include "render/gl/vertexarray.h"
#include "render/scene/surfacemesh.h"

namespace cartan::render {

class ArrowModel;

class GpuMesh {
public:
  explicit GpuMesh(const SurfaceMesh &mesh);

  void update(const SurfaceMesh &mesh, const ArrowModel &arrow);

  void draw(const gl::Program &program, const SurfaceMeshOptions &options) const;
  void drawVectors(std::size_t index, const ArrowModel &arrow) const;

private:
  struct GpuVectors {
    gl::VertexArray vertexArray;
    gl::Buffer instances;
    int count = 0;
  };

  void updateValues(const SurfaceMesh &mesh);
  void updateVectors(const SurfaceMesh &mesh, const ArrowModel &arrow);

  gl::Buffer m_buffer;
  gl::Buffer m_values;
  gl::VertexArray m_vertexArray;
  int m_count = 0;
  std::uint64_t m_version = 0;
  bool m_hasValues = false;

  std::vector<std::unique_ptr<GpuVectors>> m_vectors;
};

} // namespace cartan::render
