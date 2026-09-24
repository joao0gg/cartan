#include <QPlainTextEdit>
#include <QPointer>

#include "shell/Module.h"

namespace {

QPointer<QPlainTextEdit> messages;
QtMessageHandler previousHandler = nullptr;

void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &message) {
  if (messages) {
    QMetaObject::invokeMethod(messages, [message]() {
      messages->appendPlainText(message);
    });
  }

  if (previousHandler != nullptr) {
    previousHandler(type, context, message);
  }
}

void install(cartan::shell::Workbench &workbench) {
  messages = new QPlainTextEdit;
  messages->setReadOnly(true);

  previousHandler = qInstallMessageHandler(handleMessage);

  workbench.addPanel("Messages", messages, Qt::BottomDockWidgetArea);
}

const bool registered = cartan::shell::registerModule(40, install);

} // namespace
