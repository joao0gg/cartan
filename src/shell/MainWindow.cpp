#include "shell/MainWindow.h"

#include <QMenu>

#include "shell/Module.h"

namespace cartan::shell {

namespace {

constexpr int kDefaultWindowWidth = 1280;
constexpr int kDefaultWindowHeight = 800;

} // namespace

MainWindow::MainWindow(core::Scene &scene, QWidget *parent)
    : QMainWindow(parent), m_workbench(*this, scene) {
  setWindowTitle("Cartan");
  resize(kDefaultWindowWidth, kDefaultWindowHeight);

  installModules(m_workbench);

  QMenu *fileMenu = m_workbench.menu("&File");

  if (!fileMenu->isEmpty()) {
    fileMenu->addSeparator();
  }

  fileMenu->addAction("E&xit", this, &QWidget::close);

  qInfo("Ready");
}

} // namespace cartan::shell
