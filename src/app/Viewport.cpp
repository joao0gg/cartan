#include "app/Viewport.h"

#include <QLabel>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include <string>

Viewport::Viewport(QWidget *parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);

  // fps timer
  m_fpsLabel = new QLabel("0 FPS", this);
  m_fpsLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_fpsLabel->setStyleSheet("color: white;");
  m_fpsLabel->move(VIEWPORT_FPS_MARGIN, VIEWPORT_FPS_MARGIN);

  auto *fpsTimer = new QTimer(this);

  connect(fpsTimer, &QTimer::timeout, this, [this]() {
    m_fpsLabel->setText(QString("%1 FPS").arg(m_frameCount * 1000 / VIEWPORT_FPS_INTERVAL));
    m_fpsLabel->adjustSize();
    m_frameCount = 0;
  });

  fpsTimer->start(VIEWPORT_FPS_INTERVAL);
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

  ++m_frameCount;
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