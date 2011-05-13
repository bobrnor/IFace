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

#include "GlobalState.h"
#include "CodeEditorWidget.h"
#include "CodeEditor.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow) {

	ui->setupUi(this);

	m_currentProjectManager = NULL;

	m_statusBar = new QStatusBar(this);
	ui->topLayout->addWidget(m_statusBar);

	m_menuBar = new QMenuBar(this);  
	ui->topLayout->addWidget(m_menuBar);
	ui->topLayout->addSpacing(0);

	m_toolBar = new QToolBar(this);
	ui->topLayout->addWidget(m_toolBar);

	initStatusBar();

	initMenu();
	initToolBar();
	initSplitters();
	initProjectTree();
	initCodeTabs();
	initErrorTable();
	initShortcuts();
	initEventFilters();
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

	m_menuBar->addMenu(fileMenu);
	m_menuBar->setMaximumHeight(100);
}

void MainWindow::initSplitters() {

}

void MainWindow::initProjectTree() {

}

void MainWindow::initToolBar() {

	m_toolBar->setIconSize(QSize(16, 16));

	m_highlightToolBarAction = m_toolBar->addAction("Highlight", this, SLOT(highlightSelectedTextSlot()));
	QIcon icon = makeIconForColor(GlobalState::instance()->textHighlightColor());
	m_highlightToolBarAction->setIcon(icon);
	m_highlightMenuAction->setIcon(icon);

	m_toolBar->addAction("Change Highlight Color", this, SLOT(changeTextHighlightColor()));
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

	menu->addSeparator();

	m_highlightMenuAction = menu->addAction("Highlight", this, SLOT(highlightSelectedTextSlot()));
	menu->addAction("Change Highlight Color", this, SLOT(changeTextHighlightColor()));

	menu->addSeparator();

	m_lastProjectsMenu = menu->addMenu("Last Projects");
	updateLastProjectsMenu();

	return menu;
}

void MainWindow::updateLastProjectsMenu() {

	m_lastProjectsMenu->clear();

	foreach (QString path, GlobalState::instance()->lastProjects()) {
		QString elidedString = fontMetrics().elidedText(path, Qt::ElideMiddle, 200);
		QAction *action = m_lastProjectsMenu->addAction(elidedString, this, SLOT(openProjectFromLastSlot()));
		action->setToolTip(path);
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
	}
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
	
	QIcon icon = makeIconForColor(GlobalState::instance()->textHighlightColor());
	m_highlightToolBarAction->setIcon(icon);
	m_highlightMenuAction->setIcon(icon);
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