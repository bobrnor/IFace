#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QStringList>
#include <QAction>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);

  m_currentProjectManager = NULL;

  m_statusBar = new QStatusBar(this);
  ui->topLayout->addWidget(m_statusBar);

  m_menuBar = new QMenuBar(this);  
  ui->topLayout->addWidget(m_menuBar);

  ui->topLayout->addSpacing(0);

  initStatusBar();
  initMenu();
  initProjectTree();
  initCodeTabs();
  initErrorTable();
}

MainWindow::~MainWindow() {

  delete ui;
  if (m_currentProjectManager != NULL) {
      delete m_currentProjectManager;
  }
  if (m_menuBar != NULL) {
      delete m_menuBar;
  }
  if (m_statusBar != NULL) {
      delete m_statusBar;
  }
}

void MainWindow::initMenu() {

  QMenu *fileMenu = createFileMenu();

  m_menuBar->addMenu(fileMenu);
  m_menuBar->setMaximumHeight(100);
}

void MainWindow::initProjectTree() {}

void MainWindow::initCodeTabs() {}

void MainWindow::initErrorTable() {}

QMenu *MainWindow::createFileMenu() {

  QMenu *menu = new QMenu(tr("&File"), m_menuBar);
  QAction *action = menu->addAction("New project...");
  connect(action, SIGNAL(triggered()), this, SLOT(newProjectSlot()));

  return menu;
}

void MainWindow::initStatusBar() {

  m_statusBar->setSizeGripEnabled(false);
}

//-------------SLOTS------------------

void MainWindow::newProjectSlot() {

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite(true);
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);

  QString fileName;
  if (dialog.exec()) {
      qDebug() << dialog.selectedFiles();
      fileName = dialog.selectedFiles().at(0);

      if (m_currentProjectManager != NULL) {
          // TODO: ask about saving ald proj
          delete m_currentProjectManager;
      }

      if (QFile::exists(fileName)) {
          bool ok = QFile::remove(fileName);
          Q_ASSERT(ok);
      }
      m_currentProjectManager = new ProjectManager(fileName);
      // TODO: update all
  }
}
