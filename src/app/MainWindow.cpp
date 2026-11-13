#include "app/MainWindow.h"

#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QTreeWidget>

namespace cartan::app {

namespace {

constexpr int kDefaultWindowWidth = 1280;
constexpr int kDefaultWindowHeight = 800;

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Cartan");
  resize(kDefaultWindowWidth, kDefaultWindowHeight);

  auto *viewport = new QLabel("Viewport");
  viewport->setAlignment(Qt::AlignCenter);
  setCentralWidget(viewport);

  auto *outline = new QTreeWidget;
  outline->setHeaderHidden(true);

  auto *outlineDock = new QDockWidget("Outline", this);
  outlineDock->setWidget(outline);
  addDockWidget(Qt::LeftDockWidgetArea, outlineDock);

  auto *messages = new QPlainTextEdit;
  messages->setReadOnly(true);

  auto *messagesDock = new QDockWidget("Messages", this);
  messagesDock->setWidget(messages);
  addDockWidget(Qt::BottomDockWidgetArea, messagesDock);

  menuBar()->addMenu("&File")->addAction("E&xit", this, &QWidget::close);
}

} // namespace cartan::app
