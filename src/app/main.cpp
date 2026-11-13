#include <QApplication>

#include "app/MainWindow.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  cartan::app::MainWindow window;
  window.show();

  return app.exec();
}
