#pragma once

#include <QWidget>

class QTreeWidget;
class QTreeWidgetItem;

namespace cartan::app {

class Context;
class Properties;

class Outline : public QWidget {
public:
  explicit Outline(Context &context, QWidget *parent = nullptr);

private:
  void refresh();
  void activate(QTreeWidgetItem *item);
  void toggle(QTreeWidgetItem *item);
  void selectionChanged(QTreeWidgetItem *item);

  Context &m_context;
  QTreeWidget *m_tree = nullptr;
  Properties *m_properties = nullptr;
};

} // namespace cartan::app
