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
