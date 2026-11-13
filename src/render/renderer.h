#pragma once

#include <string>

namespace cartan::render {

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
  bool initialize(GLLoader loader);
  const GLInfo &info() const;

  void resize(int width, int height);
  void render(unsigned int framebuffer);

private:
  GLInfo m_info;
  int m_width = 0;
  int m_height = 0;
};

} // namespace cartan::render
