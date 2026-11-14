#include "render/grid.h"

#include <glad/gl.h>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>

#include "shaders/grid.frag.h"
#include "shaders/grid.vert.h"

namespace cartan::render {

namespace {

constexpr float kFadeRatio = 10.0f;

} // namespace

Grid::Grid() : m_program(grid_vert, grid_frag) {
  glGenVertexArrays(1, &m_vertexArray);
}

Grid::~Grid() {
  glDeleteVertexArrays(1, &m_vertexArray);
}

void Grid::draw(const Camera &camera, const GridStyle &style) const {
  const glm::mat4 viewProjection = camera.projection() * camera.view();
  const float distance = glm::length(camera.position - camera.target);

  m_program.use();
  m_program.set("u_viewProjection", viewProjection);
  m_program.set("u_inverseViewProjection", glm::inverse(viewProjection));
  m_program.set("u_cameraPosition", camera.position);
  m_program.set("u_fadeDistance", distance * kFadeRatio);
  m_program.set("u_cellSize", style.cellSize);
  m_program.set("u_minorColor", style.minorColor);
  m_program.set("u_majorColor", style.majorColor);
  m_program.set("u_xAxisColor", style.xAxisColor);
  m_program.set("u_zAxisColor", style.zAxisColor);
  m_program.set("u_minorAlpha", style.minorAlpha);
  m_program.set("u_majorAlpha", style.majorAlpha);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindVertexArray(m_vertexArray);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);

  glDisable(GL_BLEND);
}

} // namespace cartan::render
