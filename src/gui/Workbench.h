// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <QHash>
#include <QString>
#include <Qt>

#include "app/Scene.h"

class QMainWindow;
class QMenu;
class QWidget;

namespace cartan::gui {

class Workbench {
public:
  Workbench(QMainWindow &window, app::Scene &scene);

  QWidget *window() const;
  app::Scene &scene() const;

  QMenu *menu(const QString &title);
  void addPanel(const QString &title, QWidget *widget, Qt::DockWidgetArea area);
  void setCentralWidget(QWidget *widget);

private:
  QMainWindow &m_window;
  app::Scene &m_scene;
  QHash<QString, QMenu *> m_menus;
};

} // namespace cartan::gui
