#include "shell/MainWindow.h"

#include <QMenu>

#include "shell/Module.h"

namespace cartan::shell {

MainWindow::MainWindow(core::Scene &scene, QWidget *parent)
    : QMainWindow(parent), m_workbench(*this, scene) {
  setWindowTitle("Cartan");
  resize(CARTAN_DEFAULT_WINDOW_WIDTH, CARTAN_DEFAULT_WINDOW_HEIGHT);

  installModules(m_workbench);

  QMenu *fileMenu = m_workbench.menu("&File");

  if (!fileMenu->isEmpty()) {
    fileMenu->addSeparator();
  }

  fileMenu->addAction("E&xit", this, &QWidget::close);

  qInfo("Ready");
}

} // namespace cartan::shell
