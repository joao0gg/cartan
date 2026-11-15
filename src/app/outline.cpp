#include "app/outline.h"

#include "app/context.h"

namespace cartan::app {

namespace {

constexpr int kStructureRole = Qt::UserRole;
constexpr int kQuantityRole = Qt::UserRole + 1;

} // namespace

Outline::Outline(Context &context, QWidget *parent) : QTreeWidget(parent), m_context(context) {
  setHeaderHidden(true);

  connect(&context, &Context::sceneChanged, this, &Outline::refresh, Qt::QueuedConnection);
  connect(this, &QTreeWidget::itemClicked, this, &Outline::activate);

  refresh();
}

void Outline::refresh() {
  clear();

  for (const auto &structure : m_context.scene().structures()) {
    const QString name = QString::fromStdString(structure->name());

    auto *item = new QTreeWidgetItem({name});
    item->setData(0, kStructureRole, name);
    addTopLevelItem(item);

    const auto *mesh = dynamic_cast<const render::SurfaceMesh *>(structure.get());

    if (mesh == nullptr) {
      continue;
    }

    for (const auto &quantity : mesh->quantities()) {
      auto *child = new QTreeWidgetItem({QString::fromStdString(quantity->name())});
      child->setData(0, kStructureRole, name);
      child->setData(0, kQuantityRole, QString::fromStdString(quantity->name()));

      QFont font = child->font(0);
      font.setBold(quantity.get() == mesh->activeQuantity());
      child->setFont(0, font);

      item->addChild(child);
    }

    item->setExpanded(true);
  }
}

void Outline::activate(QTreeWidgetItem *item) {
  const std::string structure = item->data(0, kStructureRole).toString().toStdString();
  const std::string quantity = item->data(0, kQuantityRole).toString().toStdString();

  auto *mesh = dynamic_cast<render::SurfaceMesh *>(m_context.scene().find(structure));

  if (mesh == nullptr) {
    return;
  }

  mesh->setActiveQuantity(quantity);
  m_context.notifySceneChanged();
}

} // namespace cartan::app
