#include "app/log.h"

#include <QPointer>

namespace cartan::app {

namespace {

QPointer<Log> current;
QtMessageHandler previousHandler = nullptr;

} // namespace

Log::Log(QObject *parent) : QObject(parent) {
}

Log::~Log() {
  if (current == this) {
    qInstallMessageHandler(previousHandler);
    current = nullptr;
  }
}

void Log::install() {
  if (current == this) {
    return;
  }

  current = this;
  previousHandler = qInstallMessageHandler(handleMessage);
}

const QList<LogEntry> &Log::entries() const {
  return m_entries;
}

void Log::append(QtMsgType type, const QString &text) {
  m_entries.append({type, text});
  emit added(m_entries.last());
}

void Log::handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &message) {
  if (current) {
    QMetaObject::invokeMethod(current, [type, message]() {
      if (current) {
        current->append(type, message);
      }
    });
  }

  if (previousHandler != nullptr) {
    previousHandler(type, context, message);
  }
}

} // namespace cartan::app
