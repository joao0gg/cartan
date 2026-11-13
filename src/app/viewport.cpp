#include "app/viewport.h"

#include <QOpenGLContext>

namespace cartan::app {

namespace {

render::GLProc loadProc(const char *name) {
  return QOpenGLContext::currentContext()->getProcAddress(name);
}

} // namespace

Viewport::Viewport(QWidget *parent) : QOpenGLWidget(parent) {
}

void Viewport::initializeGL() {
  m_ready = m_renderer.initialize(loadProc);

  const render::GLInfo &info = m_renderer.info();

  if (!m_ready) {
    qCritical().noquote() << "OpenGL 4.1 core is not available (got"
                          << QString::fromStdString(info.version) + ")";
    return;
  }

  qInfo().noquote() << "OpenGL" << QString::fromStdString(info.version);
  qInfo().noquote() << "GPU" << QString::fromStdString(info.renderer);
}

void Viewport::resizeGL(int width, int height) {
  const qreal ratio = devicePixelRatioF();

  m_renderer.resize(qRound(width * ratio), qRound(height * ratio));
}

void Viewport::paintGL() {
  if (!m_ready) {
    return;
  }

  m_renderer.render(defaultFramebufferObject());
}

} // namespace cartan::app
