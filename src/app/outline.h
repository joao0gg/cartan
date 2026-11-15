#pragma once

#include <QTreeWidget>

namespace cartan::app {

class Context;

class Outline : public QTreeWidget {
public:
  explicit Outline(Context &context, QWidget *parent = nullptr);

private:
  void refresh();

  Context &m_context;
};

} // namespace cartan::app
