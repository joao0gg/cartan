#pragma once

#include <QMainWindow>

#define CARTAN_DEFAULT_WINDOW_WIDTH 1280
#define CARTAN_DEFAULT_WINDOW_HEIGHT 800

class QPlainTextEdit;
class Viewport;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void openFile();
  void log(const QString &message);

  Viewport *m_viewport = nullptr;
  QPlainTextEdit *m_log = nullptr;
};