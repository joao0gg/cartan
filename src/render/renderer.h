#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "render/camera/camera.h"
#include "render/colormap.h"
#include "render/scene/scene.h"
#include "render/theme.h"

namespace cartan::render {

class ArrowModel;
class Grid;
class GpuMesh;

namespace gl {
class Program;
class Texture1D;
} // namespace gl

using GLProc = void (*)();
using GLLoader = GLProc (*)(const char *name);

struct GLInfo {
  std::string vendor;
  std::string renderer;
  std::string version;
  std::string glsl;
};

class Renderer {
public:
  Renderer();
  ~Renderer();

  void initialize(GLLoader loader);
  void release();

  const GLInfo &info() const;

  void setTheme(const Theme &theme);

  void resize(int width, int height);
  void render(unsigned int framebuffer, const Camera &camera, const Scene &scene);

private:
  GLInfo m_info;
  int m_width = 0;
  int m_height = 0;

  Theme m_theme;

  void sync(const Scene &scene);
  void drawMeshes(const Camera &camera, const Scene &scene);
  void drawVectors(const Camera &camera, const Scene &scene);
  void drawGrid(const Camera &camera, const Scene &scene);

  std::unique_ptr<Grid> m_grid;
  std::unique_ptr<gl::Program> m_meshProgram;
  std::unique_ptr<gl::Program> m_arrowProgram;
  std::unique_ptr<ArrowModel> m_arrow;
  std::unordered_map<std::uint64_t, std::unique_ptr<GpuMesh>> m_meshes;
  std::unordered_map<Colormap, std::unique_ptr<gl::Texture1D>> m_colormaps;
};

} // namespace cartan::render
