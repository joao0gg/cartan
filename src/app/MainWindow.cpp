#include "app/MainWindow.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPointer>
#include <QTreeWidget>

#include "app/viewport.h"

namespace cartan::app {

namespace {

constexpr int kDefaultWindowWidth = 1280;
constexpr int kDefaultWindowHeight = 800;

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

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Cartan");
  resize(kDefaultWindowWidth, kDefaultWindowHeight);

  messages = new QPlainTextEdit;
  messages->setReadOnly(true);
  previousHandler = qInstallMessageHandler(handleMessage);

  setCentralWidget(new Viewport);

  auto *outline = new QTreeWidget;
  outline->setHeaderHidden(true);

  auto *outlineDock = new QDockWidget("Outline", this);
  outlineDock->setWidget(outline);
  addDockWidget(Qt::LeftDockWidgetArea, outlineDock);

  auto *messagesDock = new QDockWidget("Messages", this);
  messagesDock->setWidget(messages);
  addDockWidget(Qt::BottomDockWidgetArea, messagesDock);

  menuBar()->addMenu("&File")->addAction("E&xit", this, &QWidget::close);
}

} // namespace cartan::app
