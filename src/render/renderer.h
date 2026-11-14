#pragma once

#include <memory>
#include <string>

#include "render/camera/camera.h"
#include "render/theme.h"

namespace cartan::render {

class Grid;

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
  void render(unsigned int framebuffer, const Camera &camera);

private:
  GLInfo m_info;
  int m_width = 0;
  int m_height = 0;

  Theme m_theme;

  std::unique_ptr<Grid> m_grid;
};

} // namespace cartan::render
