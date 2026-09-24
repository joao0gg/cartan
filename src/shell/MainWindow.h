#pragma once

#include <QMainWindow>

#include "core/Scene.h"
#include "shell/Workbench.h"

#define CARTAN_DEFAULT_WINDOW_WIDTH 1280
#define CARTAN_DEFAULT_WINDOW_HEIGHT 800

namespace cartan::shell {

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(core::Scene &scene, QWidget *parent = nullptr);

private:
  Workbench m_workbench;
};

} // namespace cartan::shell
