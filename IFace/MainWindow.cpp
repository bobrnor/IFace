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
  initSplitters();
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

void MainWindow::initSplitters() {

}

void MainWindow::initProjectTree() {

}

void MainWindow::initCodeTabs() {

//  ui->codeTabs->widget(0)->setLayout();
//  ui->codeTabs->widget(0)->layout()->addWidget(new CodeEditor(ui->codeTabs->widget(0)));
//  ui->codeTabs->widget(0)->layout()->setMargin(0);

//  ui->codeTabs->widget(1)->setLayout(new QVBoxLayout(ui->codeTabs->widget(1)));
//  ui->codeTabs->widget(1)->layout()->addWidget(new CodeEditor(ui->codeTabs->widget(1)));
//  ui->codeTabs->widget(1)->layout()->setMargin(0);
}

void MainWindow::initErrorTable() {}

QMenu *MainWindow::createFileMenu() {

  QMenu *menu = new QMenu(tr("&File"), m_menuBar);

  QAction *action = menu->addAction("New project...");
  connect(action, SIGNAL(triggered()), this, SLOT(newProjectSlot()));

  action = menu->addAction("Open project...");
  connect(action, SIGNAL(triggered()), this, SLOT(openProjectSlot()));

  action = menu->addAction("New file...");
  connect(action, SIGNAL(triggered()), this, SLOT(newProjectFileSlot()));

  action = menu->addAction("Open file...");
  connect(action, SIGNAL(triggered()), this, SLOT(openProjectFileSlot()));

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
          // TODO: ask about saving old proj
          delete m_currentProjectManager;
      }

      if (QFile::exists(fileName)) {
          bool ok = QFile::remove(fileName);
          Q_ASSERT(ok);
      }
      m_currentProjectManager = new ProjectManager(fileName);
      m_currentProjectManager->assignTabWidget(ui->codeTabs);
	  m_currentProjectManager->assignProjectTreeWidget(ui->projectTree);
      // TODO: update all
  }
}

void MainWindow::newProjectFileSlot() {

  Q_ASSERT(m_currentProjectManager != NULL);

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

      if (QFile::exists(fileName)) {
          bool ok = QFile::remove(fileName);
          Q_ASSERT(ok);
      }

      ProjectFile *projectFile = new ProjectFile(fileName);
      m_currentProjectManager->addProjectFile(SProjectFile(projectFile));
      // TODO: update all
  }
}

void MainWindow::openProjectSlot() {

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setConfirmOverwrite(true);
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);

  QString fileName;
  if (dialog.exec()) {
      qDebug() << dialog.selectedFiles();
      fileName = dialog.selectedFiles().at(0);

      if (m_currentProjectManager != NULL) {
          // TODO: ask about saving old proj
          delete m_currentProjectManager;
      }

      m_currentProjectManager = new ProjectManager(fileName);
      m_currentProjectManager->assignTabWidget(ui->codeTabs);
	  m_currentProjectManager->assignProjectTreeWidget(ui->projectTree);
      // TODO: update all
  }
}

void MainWindow::openProjectFileSlot() {

  Q_ASSERT(m_currentProjectManager != NULL);

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setViewMode(QFileDialog::Detail);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setConfirmOverwrite(true);
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);

  QString fileName;
  if (dialog.exec()) {
      qDebug() << dialog.selectedFiles();
      fileName = dialog.selectedFiles().at(0);

      ProjectFile *projectFile = new ProjectFile(fileName);
      m_currentProjectManager->addProjectFile(SProjectFile(projectFile));
      // TODO: update all
  }
}
