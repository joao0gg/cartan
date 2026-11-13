#include <QApplication>
#include <QSurfaceFormat>

#include "app/MainWindow.h"

int main(int argc, char **argv) {
  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setSamples(4);
  QSurfaceFormat::setDefaultFormat(format);

  QApplication app(argc, argv);

  cartan::app::MainWindow window;
  window.show();

  return app.exec();
}
