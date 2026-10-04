// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <QMainWindow>

#include "app/Scene.h"
#include "gui/Workbench.h"

namespace cartan::gui {

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(app::Scene &scene, QWidget *parent = nullptr);

private:
  Workbench m_workbench;
};

} // namespace cartan::gui
