#include "app/themes.h"

#include <cmath>
#include <utility>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include <QStandardPaths>

namespace cartan::app {

namespace {

constexpr int kFormat = 1;
constexpr qint64 kMaxFileSize = 64 * 1024;
constexpr int kMaxNameLength = 64;
constexpr double kMinCellSize = 1e-6;
constexpr double kMaxCellSize = 1e6;

const QStringList kBuiltInThemes = {":/themes/light.json", ":/themes/dark.json"};

class ThemeError {
public:
  explicit ThemeError(QString message) : message(std::move(message)) {
  }

  QString message;
};

void checkKeys(const QJsonObject &object, const QSet<QString> &allowed, const QString &where) {
  for (const QString &key : object.keys()) {
    if (!allowed.contains(key)) {
      qWarning().noquote() << QString("Theme: ignoring unknown key \"%1\" in %2").arg(key, where);
    }
  }
}

void readColor(const QJsonObject &object, const QString &key, glm::vec3 &color) {
  if (!object.contains(key)) {
    return;
  }

  static const QRegularExpression pattern("^#[0-9a-fA-F]{6}$");
  const QJsonValue value = object.value(key);

  if (!value.isString() || !pattern.match(value.toString()).hasMatch()) {
    throw ThemeError(QString("\"%1\" must be a color like #rrggbb").arg(key));
  }

  const QString text = value.toString();

  for (int i = 0; i < 3; ++i) {
    color[i] = float(text.mid(1 + 2 * i, 2).toInt(nullptr, 16)) / 255.0f;
  }
}

void readNumber(const QJsonObject &object, const QString &key, double min, double max,
                float &number) {
  if (!object.contains(key)) {
    return;
  }

  const QJsonValue value = object.value(key);
  const double x = value.toDouble(std::nan(""));

  if (!value.isDouble() || !std::isfinite(x) || x < min || x > max) {
    throw ThemeError(QString("\"%1\" must be a number from %2 to %3").arg(key).arg(min).arg(max));
  }

  number = float(x);
}

QJsonObject readFile(const QString &path) {
  const QFileInfo info(path);

  if (info.isSymLink()) {
    throw ThemeError("symbolic links are not allowed");
  }

  if (info.size() > kMaxFileSize) {
    throw ThemeError("file is larger than 64 KB");
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {
    throw ThemeError(file.errorString());
  }

  const QByteArray data = file.read(kMaxFileSize + 1);

  if (data.size() > kMaxFileSize) {
    throw ThemeError("file is larger than 64 KB");
  }

  QJsonParseError error;
  const QJsonDocument document = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    throw ThemeError(error.errorString());
  }

  if (!document.isObject()) {
    throw ThemeError("top level must be an object");
  }

  const QJsonObject root = document.object();

  if (root.value("format").toInt(-1) != kFormat || !root.value("format").isDouble()) {
    throw ThemeError(QString("\"format\" must be %1").arg(kFormat));
  }

  return root;
}

QString readName(const QJsonObject &root, const QString &path) {
  const QString name = root.value("name").toString().simplified();

  if (name.isEmpty() || name.size() > kMaxNameLength) {
    return QFileInfo(path).completeBaseName();
  }

  return name;
}

render::Theme parse(const QJsonObject &root) {
  render::Theme theme;

  checkKeys(root, {"format", "name", "background", "grid"}, "the theme");
  readColor(root, "background", theme.background);

  if (root.contains("grid")) {
    if (!root.value("grid").isObject()) {
      throw ThemeError("\"grid\" must be an object");
    }

    const QJsonObject grid = root.value("grid").toObject();
    render::GridStyle &style = theme.grid;

    checkKeys(grid,
              {"minorColor", "majorColor", "xAxisColor", "zAxisColor", "minorAlpha", "majorAlpha",
               "cellSize"},
              "\"grid\"");

    readColor(grid, "minorColor", style.minorColor);
    readColor(grid, "majorColor", style.majorColor);
    readColor(grid, "xAxisColor", style.xAxisColor);
    readColor(grid, "zAxisColor", style.zAxisColor);
    readNumber(grid, "minorAlpha", 0.0, 1.0, style.minorAlpha);
    readNumber(grid, "majorAlpha", 0.0, 1.0, style.majorAlpha);
    readNumber(grid, "cellSize", kMinCellSize, kMaxCellSize, style.cellSize);
  }

  return theme;
}

} // namespace

QString themesDirectory() {
  return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/themes";
}

QString defaultThemePath() {
  return kBuiltInThemes.first();
}

QList<ThemeEntry> availableThemes() {
  QList<ThemeEntry> themes;

  const auto add = [&themes](const QString &path, bool builtIn) {
    try {
      const QJsonObject root = readFile(path);
      parse(root);
      themes.append({readName(root, path), path, builtIn});
    } catch (const ThemeError &error) {
      qWarning().noquote() << QString("Theme %1: %2").arg(path, error.message);
    }
  };

  for (const QString &path : kBuiltInThemes) {
    add(path, true);
  }

  const QDir directory(themesDirectory());
  const QFileInfoList files =
      directory.entryInfoList({"*.json"}, QDir::Files | QDir::NoSymLinks, QDir::Name);

  for (const QFileInfo &file : files) {
    add(file.absoluteFilePath(), false);
  }

  return themes;
}

std::optional<render::Theme> loadTheme(const QString &path) {
  const QFileInfo info(path);
  const bool builtIn = kBuiltInThemes.contains(path);
  const bool user =
      info.suffix() == "json" && info.absolutePath() == QDir(themesDirectory()).absolutePath();

  if (!builtIn && !user) {
    qWarning().noquote() << QString("Theme %1: not in the themes folder").arg(path);
    return std::nullopt;
  }

  try {
    return parse(readFile(path));
  } catch (const ThemeError &error) {
    qWarning().noquote() << QString("Theme %1: %2").arg(path, error.message);
    return std::nullopt;
  }
}

} // namespace cartan::app
