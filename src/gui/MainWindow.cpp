// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "gui/MainWindow.h"

#include <QMenu>

#include "gui/Module.h"

namespace cartan::gui {

namespace {

constexpr int kDefaultWindowWidth = 1280;
constexpr int kDefaultWindowHeight = 800;

} // namespace

MainWindow::MainWindow(app::Scene &scene, QWidget *parent)
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

} // namespace cartan::gui
