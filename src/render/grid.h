#pragma once

#include "render/camera/camera.h"
#include "render/gl/program.h"
#include "render/theme.h"

namespace cartan::render {

class Grid {
public:
  Grid();
  ~Grid();

  Grid(const Grid &) = delete;
  Grid &operator=(const Grid &) = delete;

  void draw(const Camera &camera, const GridStyle &style) const;

private:
  gl::Program m_program;
  unsigned int m_vertexArray = 0;
};

} // namespace cartan::render
