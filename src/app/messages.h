#pragma once

#include <QPlainTextEdit>

namespace cartan::app {

class Context;

class Messages : public QPlainTextEdit {
public:
  explicit Messages(Context &context, QWidget *parent = nullptr);
};

} // namespace cartan::app
