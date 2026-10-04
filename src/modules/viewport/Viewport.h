// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QPoint>

#include <optional>

#include "app/Scene.h"
#include "render/Camera.h"
#include "render/Renderer.h"

namespace cartan::viewport {

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

public:
  explicit Viewport(app::Scene &scene, QWidget *parent = nullptr);
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

  app::Scene &m_scene;
  app::Scene::ListenerId m_sceneListener = 0;
  std::optional<render::Renderer> m_renderer;
  render::Camera m_camera;
  bool m_uploadPending = false;
  QPoint m_lastMouse;
};

} // namespace cartan::viewport
