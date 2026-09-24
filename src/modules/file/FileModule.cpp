#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>

#include <exception>
#include <utility>

#include "core/Scene.h"
#include "io/SurfaceIO.h"
#include "shell/Module.h"

namespace {

void openFile(QWidget *window, cartan::core::Scene &scene) {
  const QString filter = QString("Mesh (%1)")
                             .arg(QString::fromStdString(cartan::io::supportedExtensions()));

  const QString path = QFileDialog::getOpenFileName(window, "Open mesh", QString(), filter);

  if (path.isEmpty()) {
    return;
  }

  try {
    auto mesh = cartan::io::loadSurface(path.toStdString());
    const QString name = QFileInfo(path).fileName();

    const QString summary = QString("Loaded \"%1\" (%2 points, %3 edges, %4 faces)")
                                .arg(name)
                                .arg(mesh.vertices.size())
                                .arg(mesh.edgeCount())
                                .arg(mesh.indices.size() / 3);

    scene.clear();
    scene.add(name.toStdString(), std::move(mesh));

    qInfo().noquote() << summary;
  }
  catch (const std::exception &error) {
    qWarning().noquote() << QString("Failed to load %1: %2").arg(path, error.what());
  }
}

void install(cartan::shell::Workbench &workbench) {
  QWidget *window = workbench.window();
  cartan::core::Scene &scene = workbench.scene();

  workbench.menu("&File")->addAction("&Open...", QKeySequence::Open, window, [window, &scene]() {
    openFile(window, scene);
  });
}

const bool registered = cartan::shell::registerModule(10, install);

} // namespace
