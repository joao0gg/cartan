#include "modules/viewport/Viewport.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <limits>
#include <string>

namespace cartan::viewport {

Viewport::Viewport(const core::Scene &scene, QWidget *parent)
    : QOpenGLWidget(parent), m_scene(scene) {
  setFocusPolicy(Qt::StrongFocus);

  connect(&m_scene, &core::Scene::changed, this, &Viewport::onSceneChanged);
}

Viewport::~Viewport() {
  makeCurrent();
  m_renderer.shutdown(*this);
  doneCurrent();
}

void Viewport::onSceneChanged() {
  m_uploadPending = true;

  if (!m_scene.objects().empty()) {
    glm::vec3 boundsMin(std::numeric_limits<float>::max());
    glm::vec3 boundsMax(std::numeric_limits<float>::lowest());

    for (const auto &object : m_scene.objects()) {
      boundsMin = glm::min(boundsMin, object.mesh.boundsMin);
      boundsMax = glm::max(boundsMax, object.mesh.boundsMax);
    }

    m_camera.frame(boundsMin, boundsMax);
  }

  update();
}

void Viewport::initializeGL() {
  initializeOpenGLFunctions();

  std::string error;
  if (!m_renderer.initialize(*this, error)) {
    qWarning("shader build failed: %s", error.c_str());
  }
}

void Viewport::paintGL() {
  if (m_uploadPending) {
    m_renderer.clearMeshes(*this);

    for (const auto &object : m_scene.objects()) {
      m_renderer.addMesh(*this, object.mesh);
    }

    m_uploadPending = false;
  }

  m_renderer.draw(*this, m_camera, static_cast<int>(width() * devicePixelRatioF()),
                  static_cast<int>(height() * devicePixelRatioF()));

  emit frameRendered();
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  m_lastMouse = event->pos();
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  const QPoint delta = event->pos() - m_lastMouse;
  m_lastMouse = event->pos();

  if (event->buttons() & Qt::LeftButton) {
    m_camera.orbit(delta.x() * VIEWPORT_ORBIT_SPEED, delta.y() * VIEWPORT_ORBIT_SPEED);
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
  m_camera.zoom(event->angleDelta().y() / VIEWPORT_WHEEL_STEP);
  update();
}

} // namespace cartan::viewport
