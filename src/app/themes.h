#pragma once

#include <optional>

#include <QList>
#include <QString>

#include "render/theme.h"

namespace cartan::app {

struct ThemeEntry {
  QString name;
  QString path;
  bool builtIn = false;
};

QString themesDirectory();
QString defaultThemePath();

QList<ThemeEntry> availableThemes();
std::optional<render::Theme> loadTheme(const QString &path);

} // namespace cartan::app
