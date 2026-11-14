#pragma once

#include <QHash>
#include <QMainWindow>

class QMenu;
class QTabWidget;

namespace cartan::app {

class Context;
struct EditorType;

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(Context &context, QWidget *parent = nullptr);

private:
  void addEditor(const QString &title, QWidget *editor, Qt::DockWidgetArea area);
  void addTab(const QString &title, QWidget *editor);
  void closeTab(int index);
  void newTab(const EditorType &type);
  void fillNewMenu(QMenu *menu, bool shortcuts);

  void openMesh();
  void fillThemeMenu(QMenu *menu);

  Context &m_context;
  QTabWidget *m_tabs = nullptr;
  QHash<QString, int> m_tabCounts;
};

} // namespace cartan::app
