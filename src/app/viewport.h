#pragma once

#include <QOpenGLWidget>

#include "render/renderer.h"

namespace cartan::app {

class Viewport : public QOpenGLWidget {
public:
  explicit Viewport(QWidget *parent = nullptr);

protected:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;

private:
  render::Renderer m_renderer;
  bool m_ready = false;
};

} // namespace cartan::app
