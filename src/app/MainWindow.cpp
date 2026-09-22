#include "app/MainWindow.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QTreeWidget>

#include <exception>

#include "app/Viewport.h"
#include "io/SurfaceIO.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Cartan");
  resize(CARTAN_DEFAULT_WINDOW_WIDTH, CARTAN_DEFAULT_WINDOW_HEIGHT);

  // File
  auto *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction("&Open...", QKeySequence::Open, this, &MainWindow::openFile);
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
  m_log = new QPlainTextEdit(this);
  m_log->setReadOnly(true);

  auto *logDock = new QDockWidget("Messages", this);
  logDock->setWidget(m_log);
  addDockWidget(Qt::BottomDockWidgetArea, logDock);

  // Viewport
  m_viewport = new Viewport(this);
  setCentralWidget(m_viewport);

  log("Ready");
}

void MainWindow::openFile() {
  const QString filter = QString("Mesh (%1)")
                             .arg(QString::fromStdString(cartan::io::supportedExtensions()));

  const QString path = QFileDialog::getOpenFileName(this, "Open mesh", QString(), filter);
  if (path.isEmpty()) {
    return;
  }

  try {
    const auto mesh = cartan::io::loadSurface(path.toStdString());
    m_viewport->setMesh(mesh);

    log(QString("Loaded \"%1\" (%2 points, %3 edges, %4 faces)")
            .arg(QFileInfo(path).fileName())
            .arg(mesh.vertices.size())
            .arg(mesh.edgeCount())
            .arg(mesh.indices.size() / 3));
  }
  catch (const std::exception &error) {
    log(QString("Failed to load %1: %2").arg(path, error.what()));
  }
}

void MainWindow::log(const QString &message) {
  m_log->appendPlainText(message);
}