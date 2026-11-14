#include "app/messages.h"

#include "app/context.h"

namespace cartan::app {

Messages::Messages(Context &context, QWidget *parent) : QPlainTextEdit(parent) {
  setReadOnly(true);

  for (const LogEntry &entry : context.log().entries()) {
    appendPlainText(entry.text);
  }

  connect(&context.log(), &Log::added, this, [this](const LogEntry &entry) {
    appendPlainText(entry.text);
  });
}

} // namespace cartan::app
