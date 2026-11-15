#pragma once

#include <cstdint>

#include "render/gl/buffer.h"
#include "render/gl/program.h"
#include "render/gl/vertexarray.h"
#include "render/scene/surfacemesh.h"

namespace cartan::render {

class GpuMesh {
public:
  explicit GpuMesh(const SurfaceMesh &mesh);

  void update(const SurfaceMesh &mesh);

  void draw(const gl::Program &program, const SurfaceMeshOptions &options) const;

private:
  gl::Buffer m_buffer;
  gl::Buffer m_values;
  gl::VertexArray m_vertexArray;
  int m_count = 0;
  std::uint64_t m_version = 0;
  bool m_hasValues = false;
};

} // namespace cartan::render
