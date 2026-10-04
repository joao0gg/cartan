// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <QPlainTextEdit>
#include <QPointer>

#include "gui/Module.h"

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

void install(cartan::gui::Workbench &workbench) {
  messages = new QPlainTextEdit;
  messages->setReadOnly(true);

  previousHandler = qInstallMessageHandler(handleMessage);

  workbench.addPanel("Messages", messages, Qt::BottomDockWidgetArea);
}

const bool registered = cartan::gui::registerModule(40, install);

} // namespace
