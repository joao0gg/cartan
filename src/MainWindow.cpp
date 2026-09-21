#include "MainWindow.h"

#include <QDockWidget>
#include <QLabel>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QTreeWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Cartan");
  resize(CARTAN_DEFAULT_WINDOW_WIDTH, CARTAN_DEFAULT_WINDOW_HEIGHT);

  // File
  auto *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction("&Open...");
  fileMenu->addSeparator();
  fileMenu->addAction("E&xit", this, &QWidget::close);

  // View
  menuBar()->addMenu("&View");

  // Outline
  auto *outline = new QTreeWidget(this);
  outline->setHeaderHidden(true);
  outline->addTopLevelItem(new QTreeWidgetItem({"Model"}));

  auto *outlineDock = new QDockWidget("Outline", this);
  outlineDock->setWidget(outline);
  addDockWidget(Qt::LeftDockWidgetArea, outlineDock);

  // Messages
  auto *log = new QPlainTextEdit(this);
  log->setReadOnly(true);

  auto *logDock = new QDockWidget("Messages", this);
  logDock->setWidget(log);
  addDockWidget(Qt::BottomDockWidgetArea, logDock);

  // Viewport
  auto *viewport = new QLabel("Viewport", this);
  viewport->setAlignment(Qt::AlignCenter);
  setCentralWidget(viewport);

  log->appendPlainText("Ready");
}