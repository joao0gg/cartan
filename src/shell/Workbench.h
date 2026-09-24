#pragma once

#include <QHash>
#include <QString>
#include <Qt>

#include "core/Scene.h"

class QMainWindow;
class QMenu;
class QWidget;

namespace cartan::shell {

class Workbench {
public:
  Workbench(QMainWindow &window, core::Scene &scene);

  QWidget *window() const;
  core::Scene &scene() const;

  QMenu *menu(const QString &title);
  void addPanel(const QString &title, QWidget *widget, Qt::DockWidgetArea area);
  void setCentralWidget(QWidget *widget);

private:
  QMainWindow &m_window;
  core::Scene &m_scene;
  QHash<QString, QMenu *> m_menus;
};

} // namespace cartan::shell
