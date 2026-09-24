#include <QTreeWidget>

#include "shell/Module.h"

namespace {

void install(cartan::shell::Workbench &workbench) {
  auto *outline = new QTreeWidget;
  outline->setHeaderHidden(true);
  outline->addTopLevelItem(new QTreeWidgetItem({"Model"}));

  workbench.addPanel("Outline", outline, Qt::LeftDockWidgetArea);
}

const bool registered = cartan::shell::registerModule(30, install);

} // namespace
