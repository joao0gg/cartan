#include "shell/Workbench.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>

namespace cartan::shell {

Workbench::Workbench(QMainWindow &window, core::Scene &scene) : m_window(window), m_scene(scene) {
}

QWidget *Workbench::window() const {
  return &m_window;
}

core::Scene &Workbench::scene() const {
  return m_scene;
}

QMenu *Workbench::menu(const QString &title) {
  QMenu *&menu = m_menus[title];

  if (menu == nullptr) {
    menu = m_window.menuBar()->addMenu(title);
  }

  return menu;
}

void Workbench::addPanel(const QString &title, QWidget *widget, Qt::DockWidgetArea area) {
  auto *dock = new QDockWidget(title, &m_window);
  dock->setWidget(widget);
  m_window.addDockWidget(area, dock);
}

void Workbench::setCentralWidget(QWidget *widget) {
  m_window.setCentralWidget(widget);
}

} // namespace cartan::shell
