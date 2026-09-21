#pragma once

#include <QMainWindow>

#define CARTAN_DEFAULT_WINDOW_WIDTH 1280
#define CARTAN_DEFAULT_WINDOW_HEIGHT 800

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
};
