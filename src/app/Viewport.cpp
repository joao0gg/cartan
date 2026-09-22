#include "app/Viewport.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <string>

Viewport::Viewport(QWidget *parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}

Viewport::~Viewport() {
  makeCurrent();
  m_renderer.shutdown(*this);
  doneCurrent();
}

void Viewport::setMesh(const cartan::render::RenderMesh &mesh) {
  m_pending = mesh;
  m_uploadPending = true;
  m_camera.frame(mesh.boundsMin, mesh.boundsMax);
  update();
}

void Viewport::initializeGL() {
  initializeOpenGLFunctions();

  std::string error;
  try {
    if (!m_renderer.initialize(*this, SHADER_DIR, error)) {
      qWarning("shader build failed: %s", error.c_str());
    }
  }
  catch (const std::exception &e) {
    qWarning("shader load failed: %s", e.what());
  }
}

void Viewport::paintGL() {
  if (m_uploadPending) {
    m_renderer.setMesh(*this, m_pending);
    m_uploadPending = false;
  }

  m_renderer.draw(*this, m_camera, static_cast<int>(width() * devicePixelRatioF()),
                  static_cast<int>(height() * devicePixelRatioF()));
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