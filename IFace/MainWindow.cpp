#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QStringList>
#include <QAction>
#include <QFile>
#include <QCloseEvent>
#include <QShortcut>
#include <QWidget>
#include <QColor>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QMessageBox>

#include "GlobalState.h"
#include "CodeEditorWidget.h"
#include "CodeEditor.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow) {

	ui->setupUi(this);

	m_currentProjectManager = NULL;
	m_highlightToolBarAction = NULL;
	m_highlightMenuAction = NULL;

	m_isInCompile = false;

	m_statusBar = new QStatusBar(this);
	ui->topLayout->addWidget(m_statusBar);

	m_menuBar = new MenuBar(this);  
	ui->topLayout->addWidget(m_menuBar);
	ui->topLayout->addSpacing(0);

	connect(m_menuBar, SIGNAL(hovered(QAction*)), this, SLOT(showStatusBarInfo(QAction*)));
	connect(m_menuBar, SIGNAL(triggered(QAction*)), this, SLOT(hideStatusBarInfo()));
	connect(m_menuBar, SIGNAL(leaveSignal()), this, SLOT(hideStatusBarInfo()));

// 	m_toolBar = new QToolBar(this);
// 	ui->topLayout->addWidget(m_toolBar);

	initStatusBar();

	initMenu();
	//initToolBar();
	initSplitters();
	initProjectTree();
	initCodeTabs();
	initErrorTable();
	initShortcuts();
	initEventFilters();

	showStartUpHelp();
	updateEnability();
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

	for (int i = m_shortcutList.count() - 1; i >= 0; --i) {
		QShortcut *shortcut = m_shortcutList.at(i);
		m_shortcutList.removeAt(i);
		delete shortcut;
	}
}

void MainWindow::showStartUpHelp() {

	if (!GlobalState::instance()->enHighFrequenceError().isEmpty()) {
		QMessageBox *messageBox = new QMessageBox("Last session hegh frequence error help",
			GlobalState::instance()->enHighFrequenceError(), 
			QMessageBox::Information, 
			QMessageBox::Ok,
			QMessageBox::NoButton, 
			QMessageBox::NoButton);
		messageBox->exec();
		delete messageBox;
	}	
}

void MainWindow::closeEvent(QCloseEvent *event) {

	if (m_currentProjectManager != NULL) {
		bool tryCloseResult = m_currentProjectManager->beginCloseProject();
		if (tryCloseResult) {
			m_currentProjectManager->endCloseProject();
			event->accept();
		}
		else {
			event->ignore();
		}
	}
	else {
		event->accept();
	}
}

void MainWindow::initMenu() {

	QMenu *fileMenu = createFileMenu();
	QMenu *editMenu = createEditMenu();
	QMenu *buildMenu = createBuildMenu();
	QMenu *languageMenu = createLanguageMenu();
	QMenu *helpMenu = createHelpMenu();

	m_menuBar->addMenu(fileMenu);
	m_menuBar->addMenu(editMenu);
	m_menuBar->addMenu(buildMenu);
	m_menuBar->addMenu(languageMenu);
	m_menuBar->addMenu(helpMenu);
	m_menuBar->setMaximumHeight(100);
}

void MainWindow::initSplitters() {

}

void MainWindow::initProjectTree() {

}

void MainWindow::initToolBar() {

	m_toolBar->setIconSize(QSize(16, 16));

	m_highlightToolBarAction = m_toolBar->addAction("Highlight", this, SLOT(highlightSelectedTextSlot()));
	m_toolBar->addAction("Change Highlight Color", this, SLOT(changeTextHighlightColor()));
}

void MainWindow::initCodeTabs() {}

void MainWindow::initErrorTable() {}

QMenu *MainWindow::createFileMenu() {

	QMenu *menu = new QMenu(tr("&File"), m_menuBar);
	QAction *action = menu->addAction("Create project...", this, SLOT(newProjectSlot()));
	m_compiletRelatedActions.append(action);
	action = menu->addAction("Open project...", this, SLOT(openProjectSlot()));
	m_compiletRelatedActions.append(action);
	menu->addSeparator();

	QMenu *addMenu = new QMenu(tr("Add"), menu);
	menu->addMenu(addMenu);

	action = addMenu->addAction("&New file...", this, SLOT(newProjectFileSlot()));
	action->setShortcut(QKeySequence("Ctrl+N"));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	action = addMenu->addAction("Existing file...", this, SLOT(openProjectFileSlot()));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);
	menu->addSeparator();

	action = menu->addAction("&Save", this, SLOT(saveCurrentFileSlot()));
	action->setShortcut(QKeySequence("Ctrl+S"));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);
	action = menu->addAction("Save all", this, SLOT(saveAllSlot()));
	action->setShortcut(QKeySequence("Ctrl+Shift+S"));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);
	
	menu->addSeparator();

	m_lastProjectsMenu = menu->addMenu("Last Projects");
	updateLastProjectsMenu();

	menu->addSeparator();

	menu->addAction("Exit", this, SLOT(close()));

	return menu;
}

QMenu *MainWindow::createEditMenu() {

	QMenu *editMenu = new QMenu("&Edit", m_menuBar);
	m_highlightMenuAction = editMenu->addAction("Highlight", this, SLOT(highlightSelectedTextSlot()));
	m_highlightMenuAction->setShortcut(QKeySequence("Ctrl+H"));
	m_editorRelatedActions.append(m_highlightMenuAction);
	m_projectRelatedActions.append(m_highlightMenuAction);

	QAction *action = editMenu->addAction("Change Highlight Color", this, SLOT(changeTextHighlightColor()));
	m_editorRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	return editMenu;
}

QMenu *MainWindow::createBuildMenu() {

	QMenu *buildMenu = new QMenu("&Build", m_menuBar);
	QAction *action = buildMenu->addAction("Compile", this, SLOT(compile()));
	action->setShortcut(QKeySequence("F5"));
	m_projectRelatedActions.append(action);
	m_compiletRelatedActions.append(action);
	return buildMenu;
}

QMenu *MainWindow::createLanguageMenu() {

	QMenu *langMenu = new QMenu("&Language", m_menuBar);
	return langMenu;
}

QMenu *MainWindow::createHelpMenu() {

	QMenu *helpMenu = new QMenu("&Help", m_menuBar);
	return helpMenu;
}

void MainWindow::updateLastProjectsMenu() {

	m_lastProjectsMenu->clear();

	foreach (QString path, GlobalState::instance()->lastProjects()) {
		QString elidedString = fontMetrics().elidedText(path, Qt::ElideMiddle, 200);
		QAction *action = m_lastProjectsMenu->addAction(elidedString, this, SLOT(openProjectFromLastSlot()));
		action->setToolTip(path);
		action->setStatusTip(path);
		connect(action, SIGNAL(hovered()), this, SLOT(showStatusBarInfo()));
	}
}

void MainWindow::initStatusBar() {

	m_statusBar->setSizeGripEnabled(false);
}

void MainWindow::initShortcuts() {

	QShortcut *shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("Ctrl+1"));
	shortcut->setEnabled(true);
	connect(shortcut, SIGNAL(activated()), this, SLOT(shortcutActivated()));
	m_shortcutList.append(shortcut);

	shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("Ctrl+2"));
	shortcut->setEnabled(true);
	connect(shortcut, SIGNAL(activated()), this, SLOT(shortcutActivated()));
	m_shortcutList.append(shortcut);

	shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("Ctrl+3"));
	shortcut->setEnabled(true);
	connect(shortcut, SIGNAL(activated()), this, SLOT(shortcutActivated()));
	m_shortcutList.append(shortcut);

	shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("Ctrl+4"));
	shortcut->setEnabled(true);
	connect(shortcut, SIGNAL(activated()), this, SLOT(shortcutActivated()));
	m_shortcutList.append(shortcut);

	shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("Ctrl+5"));
	shortcut->setEnabled(true);
	connect(shortcut, SIGNAL(activated()), this, SLOT(shortcutActivated()));
	m_shortcutList.append(shortcut);
}

void MainWindow::setupProjectEnvironment(ProjectManager *projectManager) {

	if (projectManager != NULL) {
		projectManager->assignTabWidget(ui->codeTabs);
		projectManager->assignProjectTreeWidget(ui->projectTree);
		projectManager->assignErrorWidget(ui->errorTable);

		ProjectTreeHelper *treeHelper = projectManager->projectTreeHelper();
		connect(treeHelper, SIGNAL(addNewFileToProjectSignal()), 
			this, SLOT(newProjectFileSlot()));
		connect(treeHelper, SIGNAL(addExistingFileToProjectSignal()), 
			this, SLOT(openProjectFileSlot()));
		connect(treeHelper, SIGNAL(removeFileFromProjectSignal(ProjectFile *)), 
			this, SLOT(removeProjectFileSlot(ProjectFile *)));
		connect(projectManager, SIGNAL(compileEndSignal()), this, SLOT(compileEnd()));
	}
	updateEnability();
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
			bool tryCloseResult = m_currentProjectManager->beginCloseProject();
			if (tryCloseResult) {
				m_currentProjectManager->endCloseProject();
				delete m_currentProjectManager;
			}
			else {
				return;
			}
		}

		if (QFile::exists(fileName)) {
			bool ok = QFile::remove(fileName);
			Q_ASSERT(ok);
		}
		m_currentProjectManager = new ProjectManager(fileName);
		setupProjectEnvironment(m_currentProjectManager);
		// TODO: update all
		updateLastProjectsMenu();
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

		ProjectFile *projectFile = new ProjectFile(m_currentProjectManager->project(), fileName);
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
			bool tryCloseResult = m_currentProjectManager->beginCloseProject();
			if (tryCloseResult) {
				m_currentProjectManager->endCloseProject();
				delete m_currentProjectManager;
			}
			else {
				return;
			}
		}

		m_currentProjectManager = new ProjectManager(fileName);
		setupProjectEnvironment(m_currentProjectManager);
		// TODO: update all
		updateLastProjectsMenu();
	}
}

void MainWindow::openProjectFromLastSlot() {

	if (sender() != NULL) {
		QAction *action = static_cast<QAction *>(sender());
		QString path = action->toolTip();
		if (!path.isEmpty()) {
			if (m_currentProjectManager != NULL) {
				bool tryCloseResult = m_currentProjectManager->beginCloseProject();
				if (tryCloseResult) {
					m_currentProjectManager->endCloseProject();
					delete m_currentProjectManager;
				}
				else {
					return;
				}
			}

			m_currentProjectManager = new ProjectManager(path);
			setupProjectEnvironment(m_currentProjectManager);
			// TODO: update all
			updateLastProjectsMenu();
		}
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

		ProjectFile *projectFile = new ProjectFile(m_currentProjectManager->project(), fileName);
		m_currentProjectManager->addProjectFile(projectFile);
		// TODO: update all
	}
}

void MainWindow::removeProjectFileSlot(ProjectFile *file) {

	if (m_currentProjectManager != NULL && file != NULL) {
		m_currentProjectManager->removeProjectFile(file);
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
		m_isInCompile = true;
		m_statusBar->showMessage("Compilig...");
		updateEnability();
	}
}

void MainWindow::compileEnd() {

	QMessageBox *messageBox = new QMessageBox("Compiling",
		"Compiling is done.", 
		QMessageBox::Information, 
		QMessageBox::Ok,
		QMessageBox::NoButton, 
		QMessageBox::NoButton);
	messageBox->exec();
	delete messageBox;

	m_isInCompile = false;
	m_statusBar->showMessage("");
	updateEnability();
}

void MainWindow::setFocusToMenu() {

	if (m_menuBar != NULL) {
		m_menuBar->setFocus(Qt::ShortcutFocusReason);
	}	
}

void MainWindow::shortcutActivated() {

	if (sender() != NULL) {
		QShortcut *shortcut = static_cast<QShortcut *>(sender());
		QString key = shortcut->key();
		if (key == "Ctrl+1") {
			//setFocusToMenu();
		}
		else if (key == "Ctrl+2") {
			if (m_currentProjectManager != NULL) {
				m_currentProjectManager->setFocusToProjectTree();
			}
		}
		else if (key == "Ctrl+3") {
			if (m_currentProjectManager != NULL) {
				m_currentProjectManager->setFocusToCode();
			}
		}
		else if (key == "Ctrl+4") {
			if (m_currentProjectManager != NULL) {
				m_currentProjectManager->setFocusToComments();
			}
		}
		else if (key == "Ctrl+5") {
			if (m_currentProjectManager != NULL) {
				m_currentProjectManager->setFocusToErros();
			}
		}
	}
}

void MainWindow::initEventFilters() {

	m_menuBar->installEventFilter(this);
	ui->errorTable->installEventFilter(this);
	ui->codeAndErrorsWidget->installEventFilter(this);
	ui->projectTree->installEventFilter(this);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

	if (event->type() == QEvent::FocusIn) {
		if (object->isWidgetType()) {
			QPalette p(palette());

			qDebug() << object->metaObject()->className();

			QWidget *widget = NULL;
			if (object->inherits("QMenuBar") || object->inherits("QMenu") || object->inherits("QAction")) {
				widget = ui->line;
				p.setColor(QPalette::Dark, Qt::red);
				p.setColor(QPalette::Light, Qt::red);
				p.setColor(QPalette::Mid, Qt::red);
			}
			else {
				widget = static_cast<QWidget *>(object);
				p.setColor(QPalette::Window, Qt::red);
			}
			widget->setPalette(p);
		}
	}
	else if (event->type() == QEvent::FocusOut) {
		if (object->isWidgetType()) {
			QPalette p(palette());

			QWidget *widget = NULL;
			if (object->inherits("QMenuBar") || object->inherits("QMenu") || object->inherits("QAction")) {
				widget = ui->line;
			}
			else {
				widget = static_cast<QWidget *>(object);
			}
			widget->setPalette(p);
		}
	}
	return QWidget::eventFilter(object, event);
}

void MainWindow::changeTextHighlightColor() {

	QColor color = QColorDialog::getColor(GlobalState::instance()->textHighlightColor(), this);
	GlobalState::instance()->setTextHighlightColor(color);
	
	updateColorIcons();
}

void MainWindow::highlightSelectedTextSlot() {

	if (m_currentProjectManager != NULL && m_currentProjectManager->tabsHelper() != NULL) {
		TabsHelper *tabsHelper = m_currentProjectManager->tabsHelper();
		if (tabsHelper->tabWidget() != NULL && tabsHelper->tabWidget()->currentWidget() != NULL) {
			CodeEditorWidget *widget = static_cast<CodeEditorWidget *>(tabsHelper->tabWidget()->currentWidget());
			CodeEditor *editor = widget->codeEditor();
			editor->highlightCurrentSelection(GlobalState::instance()->textHighlightColor());
		}
	}
}

QIcon MainWindow::makeIconForColor(const QColor &color) {

	QPixmap pixmap(32, 32);
	QPainter painter(&pixmap);
	painter.setPen(color);
	painter.drawRect(2, 2, 28, 28);
	painter.fillRect(2, 2, 28, 28, color);
	painter.end();

	QIcon icon(pixmap);
	return icon;
}

void MainWindow::updateColorIcons() {

	QIcon icon = makeIconForColor(GlobalState::instance()->textHighlightColor());
	if (m_highlightToolBarAction != NULL) {
		m_highlightToolBarAction->setIcon(icon);
	}
	if (m_highlightMenuAction != NULL) {
		m_highlightMenuAction->setIcon(icon);
	}
}

void MainWindow::showStatusBarInfo(QAction *action) {

	QString statusTipText = action->statusTip();
	if (statusTipText.isEmpty()) {
		statusTipText = action->text();
	}
	m_statusBar->showMessage(statusTipText.replace("&", ""));
}

void MainWindow::hideStatusBarInfo() {

	m_statusBar->showMessage("");
}

void MainWindow::updateEnability() {

	bool projectOpen = m_currentProjectManager != NULL;

	foreach (QAction *action, m_projectRelatedActions) {
		action->setEnabled(projectOpen);
	}

	if (projectOpen) {
		foreach (QAction *action, m_compiletRelatedActions) {
			action->setEnabled(!m_isInCompile);
		}
	}
}