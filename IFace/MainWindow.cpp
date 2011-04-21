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

void MainWindow::initCodeTabs() {}

void MainWindow::initErrorTable() {}

QMenu *MainWindow::createFileMenu() {

	QMenu *menu = new QMenu(tr("&File"), m_menuBar);

	menu->addAction("Create project...", this, SLOT(newProjectSlot()));
	menu->addAction("Open project...", this, SLOT(openProjectSlot()));
	menu->addSeparator();

	QMenu *addMenu = new QMenu(tr("Add"), menu);
	menu->addMenu(addMenu);

	addMenu->addAction("New file...", this, SLOT(newProjectFileSlot()));
	addMenu->addAction("Existing file...", this, SLOT(openProjectFileSlot()));
	menu->addSeparator();

	menu->addAction("Save", this, SLOT(saveCurrentFileSlot()));
	menu->addAction("Save all", this, SLOT(saveAllSlot()));

	menu->addSeparator();

	menu->addAction("Compile", this, SLOT(compile()));

	return menu;
}

void MainWindow::initStatusBar() {

	m_statusBar->setSizeGripEnabled(false);
}

void MainWindow::setupProjectEnvironment(ProjectManager *projectManager) {

	if (projectManager != NULL) {
		projectManager->assignTabWidget(ui->codeTabs);
		projectManager->assignProjectTreeWidget(ui->projectTree);
		projectManager->assignErrorWidget(ui->errorTable);
	}
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
		setupProjectEnvironment(m_currentProjectManager);
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
		m_currentProjectManager->addProjectFile(projectFile);
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
		setupProjectEnvironment(m_currentProjectManager);
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
		m_currentProjectManager->addProjectFile(projectFile);
		// TODO: update all
	}
}

void MainWindow::saveCurrentFileSlot() {

	if (m_currentProjectManager != NULL) {
		m_currentProjectManager->saveCurrentlyOpenProjectFile();
	}
}

void MainWindow::saveAllSlot() {

	if (m_currentProjectManager != NULL) {
		m_currentProjectManager->saveAll();
	}
}

void MainWindow::compile() {

	if (m_currentProjectManager != NULL) {
		m_currentProjectManager->compile();
	}
}
