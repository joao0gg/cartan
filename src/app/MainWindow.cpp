#include "app/MainWindow.h"

#include <QActionGroup>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QTabWidget>
#include <QToolButton>
#include <QUrl>

#include <exception>
#include <filesystem>

#include "app/context.h"
#include "app/editors.h"
#include "app/messages.h"
#include "app/outline.h"
#include "app/themes.h"
#include "io/obj.h"

namespace cartan::app {

namespace {

constexpr int kDefaultWindowWidth = 1280;
constexpr int kDefaultWindowHeight = 800;

} // namespace

MainWindow::MainWindow(Context &context, QWidget *parent)
    : QMainWindow(parent), m_context(context) {
  setWindowTitle("Cartan");
  resize(kDefaultWindowWidth, kDefaultWindowHeight);

  m_tabs = new QTabWidget;
  m_tabs->setDocumentMode(true);
  m_tabs->setTabsClosable(true);
  m_tabs->setMovable(true);
  setCentralWidget(m_tabs);

  connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

  auto *newTabMenu = new QMenu(this);
  fillNewMenu(newTabMenu, false);

  auto *newTabButton = new QToolButton;
  newTabButton->setText("+");
  newTabButton->setAutoRaise(true);
  newTabButton->setToolTip("New tab");
  newTabButton->setMenu(newTabMenu);
  newTabButton->setPopupMode(QToolButton::MenuButtonPopup);
  m_tabs->setCornerWidget(newTabButton, Qt::TopRightCorner);

  connect(newTabButton, &QToolButton::clicked, this, [this]() {
    newTab(editorTypes().first());
  });

  addEditor("Outline", new Outline(context), Qt::LeftDockWidgetArea);
  addEditor("Messages", new Messages(context), Qt::BottomDockWidgetArea);

  newTab(editorTypes().first());

  QMenu *fileMenu = menuBar()->addMenu("&File");
  QAction *openAction = fileMenu->addAction("&Open...", this, &MainWindow::openMesh);
  openAction->setShortcut(QKeySequence::Open);
  fileMenu->addSeparator();
  fileMenu->addAction("E&xit", this, &QWidget::close);

  QMenu *viewMenu = menuBar()->addMenu("&View");
  fillNewMenu(viewMenu->addMenu("&New"), true);
  QAction *closeTabAction = viewMenu->addAction("&Close Tab", this, [this]() {
    closeTab(m_tabs->currentIndex());
  });
  closeTabAction->setShortcut(QKeySequence::Close);
  viewMenu->addSeparator();

  QMenu *themeMenu = viewMenu->addMenu("&Theme");
  connect(themeMenu, &QMenu::aboutToShow, this, [this, themeMenu]() {
    fillThemeMenu(themeMenu);
  });
}

void MainWindow::addEditor(const QString &title, QWidget *editor, Qt::DockWidgetArea area) {
  auto *dock = new QDockWidget(title, this);
  dock->setObjectName(title);
  dock->setWidget(editor);
  addDockWidget(area, dock);
}

void MainWindow::addTab(const QString &title, QWidget *editor) {
  m_tabs->setCurrentIndex(m_tabs->addTab(editor, title));
  editor->setFocus();
}

void MainWindow::closeTab(int index) {
  QWidget *editor = m_tabs->widget(index);

  if (editor == nullptr) {
    return;
  }

  m_tabs->removeTab(index);
  editor->deleteLater();
}

void MainWindow::newTab(const EditorType &type) {
  const int count = ++m_tabCounts[type.name];
  const QString title = count == 1 ? type.name : QString("%1 %2").arg(type.name).arg(count);

  addTab(title, type.create(m_context));
}

void MainWindow::fillNewMenu(QMenu *menu, bool shortcuts) {
  for (const EditorType &type : editorTypes()) {
    QAction *action = menu->addAction(type.name, this, [this, &type]() {
      newTab(type);
    });

    if (shortcuts && &type == &editorTypes().first()) {
      action->setShortcut(QKeySequence::AddTab);
    }
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
    action->setChecked(theme.path == m_context.themePath());
    group->addAction(action);

    connect(action, &QAction::triggered, this, [this, path = theme.path]() {
      m_context.setTheme(path);
    });
  }

  menu->addSeparator();
  menu->addAction("Open themes folder", this, []() {
    const QString directory = themesDirectory();
    QDir().mkpath(directory);
    QDesktopServices::openUrl(QUrl::fromLocalFile(directory));
  });
}

} // namespace cartan::app
