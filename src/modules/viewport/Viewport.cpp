#include "modules/viewport/Viewport.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <string>

namespace cartan::viewport {

namespace {

constexpr float kViewportOrbitSpeed = 0.4f;
constexpr float kViewportWheelStep = 120.0f;

} // namespace

Viewport::Viewport(const core::Scene &scene, QWidget *parent)
    : QOpenGLWidget(parent), m_scene(scene) {
  setFocusPolicy(Qt::StrongFocus);

  connect(&m_scene, &core::Scene::changed, this, &Viewport::onSceneChanged);
}

Viewport::~Viewport() {
  makeCurrent();
  m_renderer.reset();
  doneCurrent();
}

void Viewport::onSceneChanged() {
  m_uploadPending = true;

  if (!m_scene.objects().empty()) {
    core::Bounds sceneBounds = core::bounds(m_scene.objects().front().mesh);

    for (const auto &object : m_scene.objects()) {
      const core::Bounds objectBounds = core::bounds(object.mesh);
      sceneBounds.min = glm::min(sceneBounds.min, objectBounds.min);
      sceneBounds.max = glm::max(sceneBounds.max, objectBounds.max);
    }

    m_camera.frame(glm::vec3(sceneBounds.min), glm::vec3(sceneBounds.max));
  }

  update();
}

void Viewport::initializeGL() {
  initializeOpenGLFunctions();

  std::string error;
  m_renderer = render::Renderer::create(*this, error);

  if (!m_renderer) {
    qWarning("shader build failed: %s", error.c_str());
  }
}

void Viewport::paintGL() {
  if (m_renderer) {
    if (m_uploadPending) {
      m_renderer->clearMeshes();

      for (const auto &object : m_scene.objects()) {
        m_renderer->addMesh(object.mesh);
      }

      m_uploadPending = false;
    }

    m_renderer->draw(m_camera, static_cast<int>(width() * devicePixelRatioF()),
                     static_cast<int>(height() * devicePixelRatioF()));
  }

  emit frameRendered();
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  m_lastMouse = event->pos();
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  const QPoint delta = event->pos() - m_lastMouse;
  m_lastMouse = event->pos();

  if (event->buttons() & Qt::LeftButton) {
    m_camera.orbit(delta.x() * kViewportOrbitSpeed, delta.y() * kViewportOrbitSpeed);
  }
  else if (event->buttons() & Qt::MiddleButton) {
    m_camera.pan(static_cast<float>(delta.x()), static_cast<float>(delta.y()));
  }
  else {
    return;
  }

  update();
}

void Viewport::wheelEvent(QWheelEvent *event) {
  m_camera.zoom(event->angleDelta().y() / kViewportWheelStep);
  update();
}

} // namespace cartan::viewport
