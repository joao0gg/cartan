#include "app/viewport.h"

#include <exception>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QWheelEvent>

#include "app/context.h"

namespace cartan::app {

namespace {

render::GLProc loadProc(const char *name) {
  return QOpenGLContext::currentContext()->getProcAddress(name);
}

} // namespace

Viewport::Viewport(Context &context, QWidget *parent)
    : QOpenGLWidget(parent), m_context(context), m_controller(m_camera) {
  setFocusPolicy(Qt::StrongFocus);

  m_renderer.setTheme(context.theme());

  connect(&context, &Context::themeChanged, this, [this]() {
    m_renderer.setTheme(m_context.theme());
    update();
  });
}

Viewport::~Viewport() {
  makeCurrent();
  m_renderer.release();
  doneCurrent();
}

void Viewport::initializeGL() {
  try {
    m_renderer.initialize(loadProc);
    m_ready = true;
  } catch (const std::exception &error) {
    qCritical().noquote() << error.what();
    return;
  }

  const render::GLInfo &info = m_renderer.info();

  qInfo().noquote() << "OpenGL" << QString::fromStdString(info.version);
  qInfo().noquote() << "GPU" << QString::fromStdString(info.renderer);
}

void Viewport::resizeGL(int width, int height) {
  const qreal ratio = devicePixelRatioF();

  m_renderer.resize(qRound(width * ratio), qRound(height * ratio));
  m_camera.aspect = height > 0 ? float(width) / float(height) : 1.0f;
}

void Viewport::paintGL() {
  if (!m_ready) {
    return;
  }

  m_renderer.render(defaultFramebufferObject(), m_camera);
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  m_lastPosition = event->position().toPoint();
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  const QPoint position = event->position().toPoint();
  const QPoint delta = position - m_lastPosition;
  m_lastPosition = position;

  const bool shift = event->modifiers() & Qt::ShiftModifier;

  if (event->buttons() & Qt::RightButton || (event->buttons() & Qt::LeftButton && shift)) {
    m_controller.pan(delta.x(), delta.y(), height());
  } else if (event->buttons() & Qt::LeftButton) {
    m_controller.orbit(delta.x(), delta.y());
  } else {
    return;
  }

  update();
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *) {
  fit();
}

void Viewport::wheelEvent(QWheelEvent *event) {
  m_controller.zoom(event->angleDelta().y() / 120.0f);
  update();
}

void Viewport::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_F) {
    fit();
    return;
  }

  QOpenGLWidget::keyPressEvent(event);
}

void Viewport::fit() {
  m_controller.fit(glm::vec3(0.0f), 1.0f);
  update();
}

} // namespace cartan::app
