#include "app/context.h"

#include <exception>
#include <filesystem>
#include <optional>
#include <utility>

#include <QFileInfo>

#include <QSettings>
#include <QStandardPaths>

#include "app/themes.h"
#include "io/obj.h"

namespace cartan::app {

namespace {

QSettings settings() {
  const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

  return QSettings(directory + "/settings.ini", QSettings::IniFormat);
}

} // namespace

Context::Context(QObject *parent) : QObject(parent) {
}

Log &Context::log() {
  return m_log;
}

render::Scene &Context::scene() {
  return m_scene;
}

render::SurfaceMesh *Context::openMesh(const QString &path) {
  const QFileInfo info(path);

  try {
    io::MeshData mesh = io::readObj(std::filesystem::path(path.toStdU16String()));

    qInfo().noquote() << QString("Loaded %1: %2 vertices, %3 triangles")
                             .arg(info.fileName())
                             .arg(mesh.positions.size())
                             .arg(mesh.triangles.size());

    render::SurfaceMesh &surface =
        m_scene.addSurfaceMesh(info.completeBaseName().toStdString(), std::move(mesh.positions),
                               std::move(mesh.triangles));

    emit sceneChanged();

    return &surface;
  } catch (const std::exception &error) {
    qWarning().noquote() << QString("Failed to load %1: %2").arg(info.fileName(), error.what());
    return nullptr;
  }
}

void Context::notifySceneChanged() {
  emit sceneChanged();
}

void Context::notifyDisplayChanged() {
  emit displayChanged();
}

void Context::requestFit() {
  emit fitRequested();
}

const render::Theme &Context::theme() const {
  return m_theme;
}

const QString &Context::themePath() const {
  return m_themePath;
}

bool Context::setTheme(const QString &path) {
  if (path.isEmpty()) {
    return false;
  }

  const std::optional<render::Theme> theme = loadTheme(path);

  if (!theme) {
    return false;
  }

  m_theme = *theme;
  m_themePath = path;
  settings().setValue("theme", path);

  emit themeChanged();

  return true;
}

void Context::restoreTheme() {
  if (!setTheme(settings().value("theme").toString())) {
    setTheme(defaultThemePath());
  }
}

} // namespace cartan::app
