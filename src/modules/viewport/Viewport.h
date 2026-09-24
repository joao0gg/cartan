#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QPoint>

#include "core/Scene.h"
#include "render/Camera.h"
#include "render/Renderer.h"

#define VIEWPORT_ORBIT_SPEED 0.4f
#define VIEWPORT_WHEEL_STEP 120.0f

namespace cartan::viewport {

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

public:
  explicit Viewport(const core::Scene &scene, QWidget *parent = nullptr);
  ~Viewport() override;

signals:
  void frameRendered();

protected:
  void initializeGL() override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  void onSceneChanged();

  const core::Scene &m_scene;
  render::Renderer m_renderer;
  render::Camera m_camera;
  bool m_uploadPending = false;
  QPoint m_lastMouse;
};

} // namespace cartan::viewport
