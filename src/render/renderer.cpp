#include "render/renderer.h"

#include <cmath>
#include <stdexcept>
#include <unordered_set>

#include <glad/gl.h>

#include "render/gl/program.h"
#include "render/gl/texture.h"
#include "render/gpumesh.h"
#include "render/grid.h"
#include "shaders/surfacemesh.frag.h"
#include "shaders/surfacemesh.vert.h"

namespace cartan::render {

namespace {

constexpr int kRequiredMajor = 4;
constexpr int kRequiredMinor = 1;
constexpr int kColormapSize = 256;

std::string glString(GLenum name) {
  const auto *value = reinterpret_cast<const char *>(glGetString(name));

  return value != nullptr ? value : "";
}

} // namespace

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

void Renderer::initialize(GLLoader loader) {
  const int version = gladLoadGL(loader);

  if (version == 0) {
    throw std::runtime_error("Failed to load OpenGL");
  }

  m_info.vendor = glString(GL_VENDOR);
  m_info.renderer = glString(GL_RENDERER);
  m_info.version = glString(GL_VERSION);
  m_info.glsl = glString(GL_SHADING_LANGUAGE_VERSION);

  const int major = GLAD_VERSION_MAJOR(version);
  const int minor = GLAD_VERSION_MINOR(version);

  if (major < kRequiredMajor || (major == kRequiredMajor && minor < kRequiredMinor)) {
    throw std::runtime_error("OpenGL 4.1 core is not available (got " + m_info.version + ")");
  }

  glEnable(GL_DEPTH_TEST);

  m_grid = std::make_unique<Grid>();
  m_meshProgram = std::make_unique<gl::Program>(surfacemesh_vert, surfacemesh_frag);

  for (const Colormap colormap : kColormaps) {
    std::vector<glm::vec3> colors(kColormapSize);

    for (int i = 0; i < kColormapSize; ++i) {
      colors[i] = sampleColormap(colormap, float(i) / float(kColormapSize - 1));
    }

    m_colormaps[colormap] = std::make_unique<gl::Texture1D>(colors);
  }
}

void Renderer::release() {
  m_meshes.clear();
  m_colormaps.clear();
  m_meshProgram.reset();
  m_grid.reset();
}

const GLInfo &Renderer::info() const {
  return m_info;
}

void Renderer::setTheme(const Theme &theme) {
  m_theme = theme;
}

void Renderer::resize(int width, int height) {
  m_width = width;
  m_height = height;
}

void Renderer::render(unsigned int framebuffer, const Camera &camera, const Scene &scene) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, m_width, m_height);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);

  const glm::vec3 &background = m_theme.background;
  glClearColor(background.r, background.g, background.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_grid || !m_meshProgram) {
    return;
  }

  sync(scene);
  drawMeshes(camera, scene);
  drawGrid(camera, scene);
}

void Renderer::sync(const Scene &scene) {
  std::unordered_set<std::uint64_t> alive;

  for (const auto &structure : scene.structures()) {
    const auto *mesh = dynamic_cast<const SurfaceMesh *>(structure.get());

    if (mesh == nullptr) {
      continue;
    }

    alive.insert(mesh->id());

    if (!m_meshes.contains(mesh->id())) {
      m_meshes[mesh->id()] = std::make_unique<GpuMesh>(*mesh);
    }

    m_meshes.at(mesh->id())->update(*mesh);
  }

  std::erase_if(m_meshes, [&alive](const auto &entry) {
    return !alive.contains(entry.first);
  });
}

void Renderer::drawMeshes(const Camera &camera, const Scene &scene) {
  m_meshProgram->use();
  m_meshProgram->set("u_view", camera.view());
  m_meshProgram->set("u_projection", camera.projection());

  for (const auto &structure : scene.structures()) {
    const auto *mesh = dynamic_cast<const SurfaceMesh *>(structure.get());

    if (mesh == nullptr || !mesh->visible) {
      continue;
    }

    const ScalarQuantity *quantity = mesh->activeQuantity();
    m_meshProgram->set("u_useScalar", quantity != nullptr ? 1 : 0);

    if (quantity != nullptr) {
      const auto [min, max] = quantity->range();

      m_colormaps.at(quantity->options.colormap)->bind(0);
      m_meshProgram->set("u_colormap", 0);
      m_meshProgram->set("u_min", min);
      m_meshProgram->set("u_max", max);
      m_meshProgram->set("u_invalidColor", quantity->options.invalidColor);
    }

    m_meshes.at(mesh->id())->draw(*m_meshProgram, mesh->options);
  }
}

void Renderer::drawGrid(const Camera &camera, const Scene &scene) {
  const Bounds bounds = scene.bounds();
  const float size = bounds.empty() ? 0.0f : glm::length(bounds.max - bounds.min);

  float height = 0.0f;
  float cellSize = m_theme.grid.cellSize;

  if (size > 0.0f) {
    height = bounds.min.y;
    cellSize *= std::pow(10.0f, std::floor(std::log10(size)) - 1.0f);
  }

  m_grid->draw(camera, m_theme.grid, height, cellSize);
}

} // namespace cartan::render
