#pragma once

#include <QList>
#include <QObject>
#include <QString>

namespace cartan::app {

struct LogEntry {
  QtMsgType type;
  QString text;
};

class Log : public QObject {
  Q_OBJECT

public:
  explicit Log(QObject *parent = nullptr);
  ~Log() override;

  void install();

  const QList<LogEntry> &entries() const;

signals:
  void added(const cartan::app::LogEntry &entry);

private:
  void append(QtMsgType type, const QString &text);

  static void handleMessage(QtMsgType type, const QMessageLogContext &context,
                            const QString &message);

  QList<LogEntry> m_entries;
};

} // namespace cartan::app
