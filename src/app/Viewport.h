#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QPoint>

#include "render/Camera.h"
#include "render/RenderMesh.h"
#include "render/Renderer.h"

#define VIEWPORT_ORBIT_SPEED 0.4f
#define VIEWPORT_WHEEL_STEP 120.0f
#define VIEWPORT_FPS_INTERVAL 1000 // ms
#define VIEWPORT_FPS_MARGIN 8

class QLabel;

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

public:
  explicit Viewport(QWidget *parent = nullptr);
  ~Viewport() override;

  void setMesh(const cartan::render::RenderMesh &mesh);

protected:
  void initializeGL() override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  cartan::render::Renderer m_renderer;
  cartan::render::Camera m_camera;
  cartan::render::RenderMesh m_pending;
  bool m_uploadPending = false;
  QPoint m_lastMouse;

  QLabel *m_fpsLabel = nullptr;
  int m_frameCount = 0;
};