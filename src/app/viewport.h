#pragma once

#include <QOpenGLWidget>
#include <QPoint>

#include "render/camera/camera.h"
#include "render/camera/orbitcontroller.h"
#include "render/renderer.h"

namespace cartan::app {

class Context;

class Viewport : public QOpenGLWidget {
public:
  explicit Viewport(Context &context, QWidget *parent = nullptr);
  ~Viewport() override;

protected:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private:
  void fit();

  Context &m_context;
  render::Renderer m_renderer;
  render::Camera m_camera;
  render::OrbitController m_controller;
  QPoint m_lastPosition;
  bool m_ready = false;
};

} // namespace cartan::app
