#pragma once

#include <QMainWindow>

namespace cartan::app {

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(QWidget *parent = nullptr);
};

} // namespace cartan::app
