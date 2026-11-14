#include "app/MainWindow.h"

#include <QActionGroup>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPointer>
#include <QSettings>
#include <QStandardPaths>
#include <QTreeWidget>
#include <QUrl>

#include <exception>
#include <filesystem>

#include "app/themes.h"
#include "app/viewport.h"
#include "io/obj.h"

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

QSettings settings() {
  const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

  return QSettings(directory + "/settings.ini", QSettings::IniFormat);
}

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Cartan");
  resize(kDefaultWindowWidth, kDefaultWindowHeight);

  messages = new QPlainTextEdit;
  messages->setReadOnly(true);
  previousHandler = qInstallMessageHandler(handleMessage);

  m_viewport = new Viewport;
  setCentralWidget(m_viewport);

  auto *outline = new QTreeWidget;
  outline->setHeaderHidden(true);

  auto *outlineDock = new QDockWidget("Outline", this);
  outlineDock->setWidget(outline);
  addDockWidget(Qt::LeftDockWidgetArea, outlineDock);

  auto *messagesDock = new QDockWidget("Messages", this);
  messagesDock->setWidget(messages);
  addDockWidget(Qt::BottomDockWidgetArea, messagesDock);

  QMenu *fileMenu = menuBar()->addMenu("&File");
  QAction *openAction = fileMenu->addAction("&Open...", this, &MainWindow::openMesh);
  openAction->setShortcut(QKeySequence::Open);
  fileMenu->addSeparator();
  fileMenu->addAction("E&xit", this, &QWidget::close);

  QMenu *themeMenu = menuBar()->addMenu("&View")->addMenu("&Theme");
  connect(themeMenu, &QMenu::aboutToShow, this, [this, themeMenu]() {
    fillThemeMenu(themeMenu);
  });

  if (!applyTheme(settings().value("theme").toString())) {
    applyTheme(defaultThemePath());
  }
}

void MainWindow::openMesh() {
  const QString path =
      QFileDialog::getOpenFileName(this, "Open Mesh", QString(), "Meshes (*.obj);;All Files (*)");

  if (path.isEmpty()) {
    return;
  }

  const QString name = QFileInfo(path).fileName();

  try {
    const io::MeshData mesh = io::readObj(std::filesystem::path(path.toStdU16String()));

    qInfo().noquote() << QString("Loaded %1: %2 vertices, %3 triangles")
                             .arg(name)
                             .arg(mesh.positions.size())
                             .arg(mesh.triangles.size());
  } catch (const std::exception &error) {
    qWarning().noquote() << QString("Failed to load %1: %2").arg(name, error.what());
  }
}

void MainWindow::fillThemeMenu(QMenu *menu) {
  menu->clear();

  auto *group = new QActionGroup(menu);
  bool separated = false;

  for (const ThemeEntry &theme : availableThemes()) {
    if (!theme.builtIn && !separated) {
      menu->addSeparator();
      separated = true;
    }

    QString label = theme.name;
    label.replace("&", "&&");

    QAction *action = menu->addAction(label);
    action->setCheckable(true);
    action->setChecked(theme.path == m_themePath);
    group->addAction(action);

    connect(action, &QAction::triggered, this, [this, path = theme.path]() {
      applyTheme(path);
    });
  }

  menu->addSeparator();
  menu->addAction("Open themes folder", this, []() {
    const QString directory = themesDirectory();
    QDir().mkpath(directory);
    QDesktopServices::openUrl(QUrl::fromLocalFile(directory));
  });
}

bool MainWindow::applyTheme(const QString &path) {
  if (path.isEmpty()) {
    return false;
  }

  const std::optional<render::Theme> theme = loadTheme(path);

  if (!theme) {
    return false;
  }

  m_viewport->setTheme(*theme);
  m_themePath = path;
  settings().setValue("theme", path);

  return true;
}

} // namespace cartan::app
