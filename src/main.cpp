#include <QApplication>
#include <QSurfaceFormat>

#include "app/Scene.h"
#include "gui/MainWindow.h"

int main(int argc, char **argv) {
  QSurfaceFormat format;
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setSamples(4);
  QSurfaceFormat::setDefaultFormat(format);

  QApplication app(argc, argv);

  cartan::app::Scene scene;

  cartan::gui::MainWindow window(scene);
  window.show();

  return app.exec();
}
