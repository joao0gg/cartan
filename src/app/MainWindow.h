#pragma once

#include <QMainWindow>

class QMenu;

namespace cartan::app {

class Viewport;

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void openMesh();
  void fillThemeMenu(QMenu *menu);
  bool applyTheme(const QString &path);

  Viewport *m_viewport = nullptr;
  QString m_themePath;
};

} // namespace cartan::app
