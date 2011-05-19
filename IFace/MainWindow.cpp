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
#include <QLibraryInfo>
#include <QProcess>
#include <QLocale>
#include <QRegExp>
#include <QTextCodec>

#include "GlobalState.h"
#include "CodeEditorWidget.h"
#include "CodeEditor.h"
#include "CommentsEditor.h"

QString MainWindow::m_qtAssistantEnPath = QLibraryInfo::location(QLibraryInfo::BinariesPath) 
	+ QDir::separator() 
	+ QLatin1String("assistant");

QString MainWindow::m_qtAssistantRuPath = QLibraryInfo::location(QLibraryInfo::BinariesPath) 
	+ QDir::separator() 
	+ QLatin1String("assistant");

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow) {

	ui->setupUi(this);

	m_currentProjectManager = NULL;
	m_highlightToolBarAction = NULL;
	m_highlightMenuAction = NULL;

	m_isInCompile = false;
	m_needReopenHelp = false;

	m_statusBar = new QStatusBar(this);
	ui->topLayout->addWidget(m_statusBar);

	m_menuBar = new MenuBar(this);  
	ui->topLayout->addWidget(m_menuBar);
	ui->topLayout->addSpacing(0);

	connect(m_menuBar, SIGNAL(hovered(QAction*)), this, SLOT(showStatusBarInfo(QAction*)));
	connect(m_menuBar, SIGNAL(triggered(QAction*)), this, SLOT(hideStatusBarInfo()));
	connect(m_menuBar, SIGNAL(leaveSignal()), this, SLOT(hideStatusBarInfo()));

	m_lastEnHelpSource = m_lastRuHelpSource = "";
	m_helpProcess = NULL;

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
	updateColorIcons();
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
		QMessageBox *messageBox = new QMessageBox(tr("Last session hegh frequence error help"),
			(GlobalState::instance()->langId()) ? GlobalState::instance()->ruHighFrequenceError() : GlobalState::instance()->enHighFrequenceError(), 
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

	m_highlightToolBarAction = m_toolBar->addAction(tr("Highlight"), this, SLOT(highlightSelectedTextSlot()));
	m_toolBar->addAction(tr("Change Highlight Color"), this, SLOT(changeTextHighlightColor()));
}

void MainWindow::initCodeTabs() {}

void MainWindow::initErrorTable() {}

QMenu *MainWindow::createFileMenu() {

	QMenu *menu = new QMenu(tr("&File"), m_menuBar);
	QAction *action = menu->addAction(tr("Create project..."), this, SLOT(newProjectSlot()));
	m_compiletRelatedActions.append(action);
	action = menu->addAction(tr("Open project..."), this, SLOT(openProjectSlot()));
	m_compiletRelatedActions.append(action);
	menu->addSeparator();

	QMenu *addMenu = new QMenu(tr("Add"), menu);
	menu->addMenu(addMenu);

	action = addMenu->addAction(tr("&New file..."), this, SLOT(newProjectFileSlot()));
	action->setShortcut(QKeySequence("Ctrl+N"));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	action = addMenu->addAction(tr("Existing file..."), this, SLOT(openProjectFileSlot()));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);
	menu->addSeparator();

	action = menu->addAction(tr("&Save"), this, SLOT(saveCurrentFileSlot()));
	action->setShortcut(QKeySequence("Ctrl+S"));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);
	action = menu->addAction(tr("Save all"), this, SLOT(saveAllSlot()));
	action->setShortcut(QKeySequence("Ctrl+Shift+S"));
	m_compiletRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	menu->addSeparator();

	m_lastProjectsMenu = menu->addMenu(tr("Last Projects"));
	updateLastProjectsMenu();

	menu->addSeparator();

	menu->addAction(tr("Exit"), this, SLOT(close()));

	return menu;
}

QMenu *MainWindow::createEditMenu() {

	QMenu *editMenu = new QMenu(tr("&Edit"), m_menuBar);

	QAction *action = editMenu->addAction(tr("Cut"), this, SLOT(cut()));
	m_codeRelatedActions.append(action);
	m_commentsRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	action = editMenu->addAction(tr("Copy"), this, SLOT(copy()));
	m_codeRelatedActions.append(action);
	m_commentsRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	action = editMenu->addAction(tr("Paste"), this, SLOT(paste()));
	m_codeRelatedActions.append(action);
	m_commentsRelatedActions.append(action);
	m_projectRelatedActions.append(action);

	editMenu->addSeparator();

	m_highlightMenuAction = editMenu->addAction(tr("Highlight"), this, SLOT(highlightSelectedTextSlot()));
	m_highlightMenuAction->setShortcut(QKeySequence("Ctrl+H"));
	m_codeRelatedActions.append(m_highlightMenuAction);
	m_projectRelatedActions.append(m_highlightMenuAction);

	action = editMenu->addAction(tr("Change Highlight Color"), this, SLOT(changeTextHighlightColor()));
	m_projectRelatedActions.append(action);

	return editMenu;
}

QMenu *MainWindow::createBuildMenu() {

	QMenu *buildMenu = new QMenu(tr("&Build"), m_menuBar);
	QAction *action = buildMenu->addAction(tr("Compile"), this, SLOT(compile()));
	action->setShortcut(QKeySequence("F5"));
	m_projectRelatedActions.append(action);
	m_compiletRelatedActions.append(action);
	return buildMenu;
}

QMenu *MainWindow::createLanguageMenu() {

	QMenu *langMenu = new QMenu(tr("&Language"), m_menuBar);
	langMenu->addAction(tr("Russian"), this, SLOT(ruLang()));
	langMenu->addAction(tr("English"), this, SLOT(enLang()));
	return langMenu;
}

QMenu *MainWindow::createHelpMenu() {

	QMenu *helpMenu = new QMenu(tr("&Help"), m_menuBar);
	helpMenu->addAction(tr("Help"), this, SLOT(showHelp()));
	return helpMenu;
}

void MainWindow::updateLastProjectsMenu() {

	m_lastProjectsMenu->clear();

	foreach (QString path, GlobalState::instance()->lastProjects()) {
		QString elidedString = fontMetrics().elidedText(path, Qt::ElideMiddle, 200);
		QAction *action = m_lastProjectsMenu->addAction(elidedString, this, SLOT(openProjectFromLastSlot()));
		action->setToolTip(path);
		action->setStatusTip(path);
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

	shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("F1"));
	shortcut->setEnabled(true);
	shortcut->setContext(Qt::ApplicationShortcut);
	connect(shortcut, SIGNAL(activated()), this, SLOT(showContextHelp()));
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
	dialog.setWindowTitle(tr("Create project..."));
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
	dialog.setWindowTitle(tr("New file..."));
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
	dialog.setWindowTitle(tr("Open project..."));
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

			if (QFile::exists(path)) {
				m_currentProjectManager = new ProjectManager(path);
				setupProjectEnvironment(m_currentProjectManager);
				// TODO: update all
				updateLastProjectsMenu();
			}		
			else {
				QMessageBox *messageBox = new QMessageBox(tr("File not found"),
					tr("Selected project not found."), 
					QMessageBox::Critical, 
					QMessageBox::Ok,
					QMessageBox::NoButton, 
					QMessageBox::NoButton);
				messageBox->exec();
				delete messageBox;
			}
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
	dialog.setWindowTitle(tr("Existing file..."));
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
		m_statusBar->showMessage(tr("Compilig..."));
		updateEnability();
	}
}

void MainWindow::compileEnd() {

	QMessageBox *messageBox = new QMessageBox(tr("Compiling"),
		tr("Compiling is done."), 
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
	GlobalState::instance()->setEventFilter(this);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

	static int i = 0;
	if (event->type() == QEvent::FocusIn) {

		if (object->isWidgetType()) {
			QPalette p(palette());

			QWidget *widget = NULL;
			if (object->inherits("QMenuBar") || object->inherits("QMenu") || object->inherits("QAction")) {
				widget = ui->line;
				p.setColor(QPalette::Dark, Qt::red);
				p.setColor(QPalette::Light, Qt::red);
				p.setColor(QPalette::Mid, Qt::red);
			}
			else {
				m_isInCode = object->inherits("CodeEditor");
				m_isInComments = object->inherits("CommentsEditor");
				updateEnability();
				if (!m_isInCode && !m_isInComments) {
					widget = static_cast<QWidget *>(object);
					p.setColor(QPalette::Window, Qt::red);
				}
			}
			if (widget != NULL) {
				widget->setPalette(p);
			}
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
	else if (event->type() == QEvent::LanguageChange) {
		qDebug() << __FUNCSIG__;
		i++;
		if (i == 4) {
			ui->retranslateUi(this);
			retranslate();
			i = 0;
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
			m_currentProjectManager->setFocusToCode();
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

		foreach (QAction *action, m_codeRelatedActions) {
			action->setEnabled(m_isInCode);
		}

		if (m_isInComments) {
			foreach (QAction *action, m_commentsRelatedActions) {
				action->setEnabled(m_isInComments);
			}
		}
	}
}

void MainWindow::paste() {

	if (m_currentProjectManager != NULL && m_currentProjectManager->tabsHelper() != NULL) {
		TabsHelper *tabsHelper = m_currentProjectManager->tabsHelper();
		if (tabsHelper->tabWidget() != NULL && tabsHelper->tabWidget()->currentWidget() != NULL) {
			CodeEditorWidget *widget = static_cast<CodeEditorWidget *>(tabsHelper->tabWidget()->currentWidget());
			if (m_isInCode) {
				CodeEditor *editor = widget->codeEditor();
				editor->paste();
				m_currentProjectManager->setFocusToCode();
			}
			else if (m_isInComments) {
				CommentsEditor *editor = widget->commentsArea();
				editor->paste();
				m_currentProjectManager->setFocusToComments();
			}
		}
	}
}

void MainWindow::cut() {

	if (m_currentProjectManager != NULL && m_currentProjectManager->tabsHelper() != NULL) {
		TabsHelper *tabsHelper = m_currentProjectManager->tabsHelper();
		if (tabsHelper->tabWidget() != NULL && tabsHelper->tabWidget()->currentWidget() != NULL) {
			CodeEditorWidget *widget = static_cast<CodeEditorWidget *>(tabsHelper->tabWidget()->currentWidget());
			if (m_isInCode) {
				CodeEditor *editor = widget->codeEditor();
				editor->cut();
				m_currentProjectManager->setFocusToCode();
			}
			else if (m_isInComments) {
				CommentsEditor *editor = widget->commentsArea();
				editor->cut();
				m_currentProjectManager->setFocusToComments();
			}
		}
	}
}

void MainWindow::copy() {

	if (m_currentProjectManager != NULL && m_currentProjectManager->tabsHelper() != NULL) {
		TabsHelper *tabsHelper = m_currentProjectManager->tabsHelper();
		if (tabsHelper->tabWidget() != NULL && tabsHelper->tabWidget()->currentWidget() != NULL) {
			CodeEditorWidget *widget = static_cast<CodeEditorWidget *>(tabsHelper->tabWidget()->currentWidget());
			if (m_isInCode) {
				CodeEditor *editor = widget->codeEditor();
				editor->copy();
				m_currentProjectManager->setFocusToCode();
			}
			else if (m_isInComments) {
				CommentsEditor *editor = widget->commentsArea();
				editor->copy();
				m_currentProjectManager->setFocusToComments();
			}			
		}
	}
}

void MainWindow::enLang() {

	GlobalState::instance()->translator()->load("iface_en.qm", ".");
	GlobalState::instance()->setLangId(0);
	qApp->installTranslator(GlobalState::instance()->translator());
	qApp->removeTranslator(GlobalState::instance()->qtTranslator());
	//ui->retranslateUi(this);
}

void MainWindow::ruLang() {

	GlobalState::instance()->translator()->load("iface_ru.qm", ".");
	GlobalState::instance()->setLangId(1);
	qApp->installTranslator(GlobalState::instance()->qtTranslator());
	qApp->installTranslator(GlobalState::instance()->translator());
	//ui->retranslateUi(this);
}

void MainWindow::retranslate() {

	qDebug() << __FUNCSIG__;
	m_menuBar->clear();
	initMenu();
	updateColorIcons();
	updateEnability();

	if (m_currentProjectManager != NULL) {
		m_currentProjectManager->retranslate();
	}

	if (m_helpProcess != NULL && m_helpProcess->state() != QProcess::NotRunning) {
		m_needReopenHelp = true;
		reopenHelp();
	}
}

void MainWindow::showRuSource(QString source) {

	if (m_helpProcess == NULL || m_helpProcess->state() == QProcess::NotRunning || m_needReopenHelp) {
		if (m_helpProcess != NULL) {
			delete m_helpProcess;
		}

		m_helpProcess = new QProcess();

		QString app = (GlobalState::instance()->langId()) ? m_qtAssistantRuPath : m_qtAssistantEnPath;
		QLatin1String qhc("C:/Documents and Settings/Administrator/gitProjs/IFace/help/help.qhc");

		QStringList args;
		args << QLatin1String("-collectionFile")
			<< qhc
			<< QLatin1String("-enableRemoteControl");

		m_helpProcess->start(app, args);

		if (!m_helpProcess->waitForStarted()) {
			QMessageBox::critical(0, QObject::tr("Simple Text Viewer"),
				QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
		}
		m_needReopenHelp = false;
	}

	//m_lastHelpSource = source;

	QString helpPath = "qthelp://mycompany.com.iface.1_0/doc/ru/";

	QByteArray ba;
	ba.append("setCurrentFilter iface_ru; ");
	ba.append("setSource " + helpPath + source + "\n");
	m_helpProcess->write(ba);
}

void MainWindow::showEnSource(QString source) {

	if (m_helpProcess == NULL || m_helpProcess->state() == QProcess::NotRunning || m_needReopenHelp) {
		if (m_helpProcess != NULL) {
			delete m_helpProcess;
		}

		m_helpProcess = new QProcess();

		QString app = (GlobalState::instance()->langId()) ? m_qtAssistantRuPath : m_qtAssistantEnPath;
		QLatin1String qhc("C:/Documents and Settings/Administrator/gitProjs/IFace/help/help.qhc");

		QStringList args;
		args << QLatin1String("-collectionFile")
			<< qhc
			<< QLatin1String("-enableRemoteControl");

		m_helpProcess->start(app, args);

		if (!m_helpProcess->waitForStarted()) {
			QMessageBox::critical(0, QObject::tr("Simple Text Viewer"),
				QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
		}
		m_needReopenHelp = false;
	}

	//m_lastHelpSource = source;

	QString helpPath = "qthelp://mycompany.com.iface.1_0/doc/en/";

	QByteArray ba;
	ba.append("setCurrentFilter iface_en; ");
	ba.append("setSource " + helpPath + source + "\n");
	m_helpProcess->write(ba);
}

void MainWindow::reopenHelp() {

	if (GlobalState::instance()->langId()) {
		showRuSource(m_lastRuHelpSource);
	}
	else {
		showEnSource(m_lastEnHelpSource);
	}
}

void MainWindow::showHelp() {

	m_lastEnHelpSource = m_lastRuHelpSource = "keywords.html";
	if (GlobalState::instance()->langId()) {
		showRuSource("keywords.html");
	}
	else {
		showEnSource("keywords.html");
	}
}

void MainWindow::showContextHelp() {

	qDebug() << __FUNCSIG__;
	if (this->focusWidget() != NULL) {
		QWidget *widgetInFocus = focusWidget();
		QWidget *widgetUnderCursor = qApp->widgetAt(QCursor::pos());
		if (QString(widgetUnderCursor->metaObject()->className()).trimmed() == "QWidget") {
			widgetUnderCursor = widgetUnderCursor->parentWidget();
		}

		if (widgetInFocus != NULL) {
			qDebug() << "Widget in focus: " << widgetInFocus->metaObject()->className();
		}
		if (widgetUnderCursor != NULL) {
			qDebug() << "Widget under cursor: " << widgetUnderCursor->metaObject()->className();
		}

		if (widgetInFocus->inherits("CodeEditor")) {
			loadKeywordHelp();
		}
		else if (widgetInFocus->inherits("QTableView")) {
			loadErrorHelp();
		}

// 		QWidget *widget = qApp->widgetAt(QCursor::pos());
// 		widget = widget->parentWidget();
// 		if (widget != NULL) {
// 			qDebug() << "Widget under cursor: " << widget->metaObject()->className();
// 		}
// 		else {
// 			qDebug() << "No widget under cursor";
// 		}
// 
// 		QWidget *_widget = focusWidget();
// 		if (_widget != NULL) {
// 			qDebug() << "Widget under focus: " << _widget->metaObject()->className();
// 		}
// 		else {
// 			qDebug() << "No widget under focus";
// 		}
// 
// 		if (widget->inherits(_widget->metaObject()->className())) {
// 			qDebug() << "inherits";
// 		}
// 		else {
// 			qDebug() << "not inherits";
// 		}
	}
}

void MainWindow::loadMenuHelp() {

}

void MainWindow::loadKeywordHelp() {
	
	QWidget *widget = m_currentProjectManager->tabsHelper()->tabWidget()->currentWidget();
	if (widget != NULL) {
		CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(widget);
		CodeEditor *codeEditor = codeEditorWidget->codeEditor();
		QString word = codeEditor->wordUnderCursor();

		qDebug() << "Word under cursor: " << word;

		if (!word.isEmpty()) {
			m_lastEnHelpSource = m_lastRuHelpSource = word + ".html";
			if (GlobalState::instance()->langId()) {
				showRuSource(m_lastRuHelpSource);
			}
			else {
				showEnSource(m_lastEnHelpSource);
			}
		}
	}
}

void MainWindow::loadCodeEditorHelp() {

}

void MainWindow::loadErrorHelp() {

	static QRegExp rx1("(\\W|\\d)");
	static QRegExp rx2("(\\[.*\\]$)");
	static QRegExp rx3("(^[^:]*: )");

	if (m_currentProjectManager != NULL && m_currentProjectManager->errorTableHelper() != NULL) {
		ErrorTableHelper *errorHelper = m_currentProjectManager->errorTableHelper();
		CompileError enError = errorHelper->currentEnError();
		QString source = enError.errorPattern();
		source.replace("<", "");
		source.replace(">", "");
		source.replace(rx2, "");
		source.replace(rx3, "");
		source.replace(rx1, "");		
		source = trasliterate(source);
		source = source.toLower() + ".html";
		m_lastEnHelpSource = source;

		CompileError ruError = errorHelper->currentRuError();
		source = ruError.errorPattern();
		source.replace("<", "");
		source.replace(">", "");
		source.replace(rx2, "");
		source.replace(rx3, "");
		source.replace(rx1, "");		
		source = trasliterate(source);
		source = source.toLower() + ".html";
		m_lastRuHelpSource = source;

		if (GlobalState::instance()->langId()) {
			showRuSource(m_lastRuHelpSource);
		}
		else {
			showEnSource(m_lastEnHelpSource);
		}
	}
}

QString MainWindow::trasliterate(QString rusText) {

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QHash<QString, QString> dict;
	QString eng;

	eng = "abcdefghijklmnopqrstuvwxyz1234567890";
	dict["а"] = "a";
	dict["б"] = "b";
	dict["в"] = "v";
	dict["г"] = "g";
	dict["д"] = "d";
	dict["е"] = "e";
	dict["ё"] = "io";
	dict["ж"] = "j";
	dict["з"] = "z";
	dict["и"] = "i";
	dict["й"] = "j";
	dict["к"] = "k";
	dict["л"] = "l";
	dict["м"] = "m";
	dict["н"] = "n";
	dict["о"] = "o";
	dict["п"] = "p";
	dict["р"] = "r";
	dict["с"] = "s";
	dict["т"] = "t";
	dict["у"] = "u";
	dict["ф"] = "f";
	dict["х"] = "h";
	dict["ц"] = "c";
	dict["ч"] = "ch";
	dict["ш"] = "sch";
	dict["щ"] = "sch";
	dict["ы"] = "i";
	dict["э"] = "e";
	dict["ю"] = "yu";
	dict["я"] = "ja";

	QString res;
	foreach (QChar ch, rusText.toLower())
		res += dict.contains(ch) ? dict[ch] : eng.contains(ch) ? ch : QString("");
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("1251"));
	return res;
}