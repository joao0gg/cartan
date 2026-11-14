#pragma once

#include <QObject>
#include <QString>

#include "app/log.h"
#include "render/theme.h"

namespace cartan::app {

class Context : public QObject {
  Q_OBJECT

public:
  explicit Context(QObject *parent = nullptr);

  Log &log();

  const render::Theme &theme() const;
  const QString &themePath() const;

  bool setTheme(const QString &path);
  void restoreTheme();

signals:
  void themeChanged();

private:
  Log m_log;
  render::Theme m_theme;
  QString m_themePath;
};

} // namespace cartan::app
