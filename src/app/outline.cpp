#include "app/outline.h"

#include "app/context.h"

namespace cartan::app {

Outline::Outline(Context &context, QWidget *parent) : QTreeWidget(parent), m_context(context) {
  setHeaderHidden(true);

  connect(&context, &Context::sceneChanged, this, &Outline::refresh);

  refresh();
}

void Outline::refresh() {
  clear();

  for (const auto &structure : m_context.scene().structures()) {
    addTopLevelItem(new QTreeWidgetItem({QString::fromStdString(structure->name())}));
  }
}

} // namespace cartan::app
