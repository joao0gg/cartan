#include "app/context.h"

#include <optional>

#include <QSettings>
#include <QStandardPaths>

#include "app/themes.h"

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

void Context::notifySceneChanged() {
  emit sceneChanged();
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
