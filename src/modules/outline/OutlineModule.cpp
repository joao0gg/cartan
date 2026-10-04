// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <QString>
#include <QTreeWidget>

#include "app/Scene.h"
#include "gui/Module.h"

namespace {

void refresh(QTreeWidget *outline, const cartan::app::Scene &scene) {
  outline->clear();

  for (const auto &object : scene.objects()) {
    outline->addTopLevelItem(new QTreeWidgetItem({QString::fromStdString(object.name)}));
  }
}

void install(cartan::gui::Workbench &workbench) {
  cartan::app::Scene &scene = workbench.scene();

  auto *outline = new QTreeWidget;
  outline->setHeaderHidden(true);
  refresh(outline, scene);

  const auto listener = scene.subscribe([outline, &scene]() {
    refresh(outline, scene);
  });

  QObject::connect(outline, &QObject::destroyed, [&scene, listener]() {
    scene.unsubscribe(listener);
  });

  workbench.addPanel("Outline", outline, Qt::LeftDockWidgetArea);
}

const bool registered = cartan::gui::registerModule(30, install);

} // namespace
