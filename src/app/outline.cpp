#include "app/outline.h"

#include <QSignalBlocker>
#include <QSplitter>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "app/context.h"
#include "app/properties.h"

namespace cartan::app {

namespace {

constexpr int kStructureRole = Qt::UserRole;
constexpr int kQuantityRole = Qt::UserRole + 1;
constexpr int kVectorRole = Qt::UserRole + 2;

} // namespace

Outline::Outline(Context &context, QWidget *parent) : QWidget(parent), m_context(context) {
  m_tree = new QTreeWidget;
  m_tree->setHeaderHidden(true);

  m_properties = new Properties(context);

  auto *splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(m_tree);
  splitter->addWidget(m_properties);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 0);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(splitter);

  connect(&context, &Context::sceneChanged, this, &Outline::refresh, Qt::QueuedConnection);
  connect(m_tree, &QTreeWidget::itemClicked, this, &Outline::activate);
  connect(m_tree, &QTreeWidget::itemChanged, this, &Outline::toggle);
  connect(m_tree, &QTreeWidget::currentItemChanged, this, &Outline::selectionChanged);

  refresh();
}

void Outline::refresh() {
  const QSignalBlocker blocker(m_tree);

  QTreeWidgetItem *current = m_tree->currentItem();
  const QString selectedStructure =
      current != nullptr ? current->data(0, kStructureRole).toString() : QString();
  const QString selectedQuantity =
      current != nullptr ? current->data(0, kQuantityRole).toString() : QString();

  QTreeWidgetItem *selected = nullptr;

  const auto remember = [&](QTreeWidgetItem *item) {
    if (item->data(0, kStructureRole).toString() == selectedStructure &&
        item->data(0, kQuantityRole).toString() == selectedQuantity) {
      selected = item;
    }
  };

  m_tree->clear();

  for (const auto &structure : m_context.scene().structures()) {
    const QString name = QString::fromStdString(structure->name());

    auto *item = new QTreeWidgetItem({name});
    item->setData(0, kStructureRole, name);
    m_tree->addTopLevelItem(item);
    remember(item);

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
      remember(child);
    }

    for (const auto &quantity : mesh->vectorQuantities()) {
      auto *child = new QTreeWidgetItem({QString::fromStdString(quantity->name())});
      child->setData(0, kStructureRole, name);
      child->setData(0, kQuantityRole, QString::fromStdString(quantity->name()));
      child->setData(0, kVectorRole, true);
      child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
      child->setCheckState(0, quantity->options.enabled ? Qt::Checked : Qt::Unchecked);

      item->addChild(child);
      remember(child);
    }

    item->setExpanded(true);
  }

  m_tree->setCurrentItem(selected);
  selectionChanged(selected);
}

void Outline::selectionChanged(QTreeWidgetItem *item) {
  if (item == nullptr) {
    m_properties->select({}, {});
    return;
  }

  m_properties->select(item->data(0, kStructureRole).toString().toStdString(),
                       item->data(0, kQuantityRole).toString().toStdString());
}

void Outline::activate(QTreeWidgetItem *item) {
  if (item->data(0, kVectorRole).toBool()) {
    return;
  }

  const std::string structure = item->data(0, kStructureRole).toString().toStdString();
  const std::string quantity = item->data(0, kQuantityRole).toString().toStdString();

  auto *mesh = dynamic_cast<render::SurfaceMesh *>(m_context.scene().find(structure));

  if (mesh == nullptr) {
    return;
  }

  mesh->setActiveQuantity(quantity);
  m_context.notifySceneChanged();
}

void Outline::toggle(QTreeWidgetItem *item) {
  if (!item->data(0, kVectorRole).toBool()) {
    return;
  }

  const std::string structure = item->data(0, kStructureRole).toString().toStdString();
  const std::string name = item->data(0, kQuantityRole).toString().toStdString();

  auto *mesh = dynamic_cast<render::SurfaceMesh *>(m_context.scene().find(structure));
  render::VectorQuantity *quantity = mesh != nullptr ? mesh->findVectorQuantity(name) : nullptr;

  if (quantity == nullptr) {
    return;
  }

  quantity->options.enabled = item->checkState(0) == Qt::Checked;
  m_context.notifyDisplayChanged();
}

} // namespace cartan::app
