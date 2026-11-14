#pragma once

#include <functional>

#include <QList>
#include <QString>

class QWidget;

namespace cartan::app {

class Context;

struct EditorType {
  QString name;
  std::function<QWidget *(Context &)> create;
};

const QList<EditorType> &editorTypes();

} // namespace cartan::app
