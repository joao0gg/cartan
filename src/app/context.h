#pragma once

#include <QObject>
#include <QString>

#include "app/colorbar.h"
#include "app/log.h"
#include "render/scene/scene.h"
#include "render/theme.h"

namespace cartan::app {

class Context : public QObject {
  Q_OBJECT

public:
  explicit Context(QObject *parent = nullptr);

  Log &log();

  render::Scene &scene();
  render::SurfaceMesh *openMesh(const QString &path);
  void notifySceneChanged();
  void requestFit();

  bool showColorBar() const;
  void setShowColorBar(bool show);

  const ColorBarStyle &colorBarStyle() const;
  void setColorBarStyle(const ColorBarStyle &style, bool save);

  const render::Theme &theme() const;
  const QString &themePath() const;

  bool setTheme(const QString &path);
  void restoreTheme();

signals:
  void themeChanged();
  void sceneChanged();
  void fitRequested();

private:
  Log m_log;
  render::Scene m_scene;
  render::Theme m_theme;
  QString m_themePath;
  bool m_showColorBar = true;
  ColorBarStyle m_colorBarStyle;
};

} // namespace cartan::app
