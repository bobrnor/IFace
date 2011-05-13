#include "ProjectManager.h"

#include <QDebug>
#include <QAction>
#include <QWidget>

#include "CodeEditorWidget.h"
#include "CodeEditor.h"
#include "GlobalState.h"
#include "CommentsEditor.h"

Q_DECLARE_METATYPE(ProjectFile *)

ProjectManager::ProjectManager() {

	m_project = NULL;
	m_tabsHelper = NULL;
	m_errorTableHelper = new ErrorTableHelper();

	connect(m_errorTableHelper, SIGNAL(errorPositionChangedSignal(ProjectFile*, int, int)), 
		this, SLOT(errorPositionChangedSlot(ProjectFile*, int, int)));
}

ProjectManager::ProjectManager(const QString &path) {

	if (!path.isEmpty()) {
		m_project = new Project(path);
	}
	else {
		ProjectManager::ProjectManager();
	}
	m_errorTableHelper = new ErrorTableHelper();
	connect(m_errorTableHelper, SIGNAL(errorPositionChangedSignal(ProjectFile*, int, int)), 
		this, SLOT(errorPositionChangedSlot(ProjectFile*, int, int)));
}

ProjectManager::~ProjectManager() {

	if (m_project != NULL) {
		delete m_project;
	}
}

bool ProjectManager::createProject(const QString &path) {

	if (m_project == NULL && path.length() > 0) {
		m_project = new Project(path);
		return true;
	}
	return false;
}

void ProjectManager::addProjectFile(ProjectFile *projectFile) {

	m_project->addProjectFile(projectFile);
	m_project->saveProjectFile(projectFile);

	if (m_tabsHelper != NULL) {
		m_tabsHelper->showTabWithFile(projectFile);
	}
	if (m_projectTreeHelper != NULL) {
		m_projectTreeHelper->update();
	}
}

void ProjectManager::removeProjectFile(ProjectFile *projectFile) {

	m_project->removeProjectFile(projectFile);
	m_project->saveUpdatedStructure();
	if (m_projectTreeHelper != NULL) {
		m_projectTreeHelper->update();
	}
}

void ProjectManager::assignTabWidget(QTabWidget *tabWidget) {

	if (m_tabsHelper != NULL) {
		disconnect(m_tabsHelper);
	}

	if (tabWidget != NULL) {
		m_tabsHelper = new TabsHelper(tabWidget);
		connect(m_tabsHelper, SIGNAL(codeCursorChangedSignal(ProjectFile*, int, int)), 
			m_errorTableHelper, SLOT(codeCursorChangedSlot(ProjectFile*, int, int)));

		connect(m_tabsHelper, SIGNAL(codeChangedSignal(ProjectFile*, int, int)), 
			m_errorTableHelper, SLOT(codeChangedSlot(ProjectFile*, int, int)));
	}
}

void ProjectManager::assignProjectTreeWidget(QTreeWidget *projectTree) {

	if (projectTree != NULL) {
		m_projectTreeHelper = new ProjectTreeHelper(projectTree);
		m_projectTreeHelper->setProject(m_project);
		connect(m_projectTreeHelper->linkedTreeWidget(), SIGNAL(itemActivated(QTreeWidgetItem*, int)), 
			this, SLOT(intemActivatedSlot(QTreeWidgetItem*)));
	}
}

ProjectFile *ProjectManager::currentlyOpenProjectFile() {

	if (m_tabsHelper != NULL) {
		return m_tabsHelper->currentlyOpenProjectFile();
	}
	else {
		return NULL;
	}
}

void ProjectManager::saveAll() {

	if (m_project != NULL) {
		m_project->saveProject();
	}
}

void ProjectManager::saveCurrentlyOpenProjectFile() {

	if (m_project != NULL) {
		ProjectFile *currentFile = currentlyOpenProjectFile();
		if (currentFile != NULL) {
			m_project->saveProjectFile(currentFile);
		}
	}
}

void ProjectManager::compile() {

	if (m_project != NULL) {
		QList<ProjectFile *> files = m_project->projectFiles();
		QMap<uint, ProjectFile *> filesToCompile = m_tabsHelper->openFiles();
		QList<ProjectFile *> projectFileList = m_project->projectFiles();

		foreach (ProjectFile *projectFile, projectFileList) {
			if (!filesToCompile.contains(projectFile->hash())) {
				filesToCompile[projectFile->hash()] = projectFile;
			}
		}

		m_compileHelper = new CompilerHelper(filesToCompile);
		connect(m_compileHelper, SIGNAL(compileCompleteSignal()), 
			this, SLOT(compileCompleteSlot()));
		m_compileHelper->compile();
	}
}

void ProjectManager::assignErrorWidget(QTableView *errorWidget) { 

	m_errorTableHelper->setErrorTable(errorWidget);
}

void ProjectManager::intemActivatedSlot(QTreeWidgetItem *item) {

	if (item != NULL) {
		QVariant data = item->data(0, Qt::UserRole);
		if (!data.isNull() && data.isValid()) {
			ProjectFile *projectFile = data.value<ProjectFile *>();
			m_tabsHelper->showTabWithFile(projectFile);
		}
	}
}

void ProjectManager::compileCompleteSlot() {

	qDebug() << "Compile compete!";
	delete m_compileHelper;

	QList<CompileError> ruErrors;
	QList<CompileError> enErrors;
	foreach (ProjectFile *projectFile, m_project->projectFiles()) {
		ruErrors.append(projectFile->compileErrorsRu());
		enErrors.append(projectFile->compileErrorsEn());
	}
	m_errorTableHelper->setErrorLists(ruErrors, enErrors);
	GlobalState::instance()->addErrors(enErrors, ruErrors);
	if (m_tabsHelper->tabWidget()->currentWidget() != NULL) {
		((CodeEditorWidget *) m_tabsHelper->tabWidget()->currentWidget())->codeEditor()->update();
	}
	emit compileEndSignal();
}

void ProjectManager::errorPositionChangedSlot(ProjectFile *file, int xPos, int yPos) {

	if (m_tabsHelper != NULL) {
		m_tabsHelper->showTabWithFile(file);
		CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(m_tabsHelper->tabWidget()->currentWidget());
		CodeEditor *codeEditor = codeEditorWidget->codeEditor();
		codeEditor->errorPositionChangedSlot(xPos, yPos);
	}
}

bool ProjectManager::beginCloseProject() {

	return m_tabsHelper->tryCloseAll();
}

void ProjectManager::endCloseProject() {

	//saveAll();
}

void ProjectManager::setFocusToCode() {

	QWidget *currentWidget = m_tabsHelper->tabWidget()->currentWidget();
	if (currentWidget != NULL) {
		CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(currentWidget);
		codeEditorWidget->codeEditor()->setFocus(Qt::ShortcutFocusReason);
	}
}

void ProjectManager::setFocusToComments() {

	QWidget *currentWidget = m_tabsHelper->tabWidget()->currentWidget();
	if (currentWidget != NULL) {
		CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(currentWidget);
		codeEditorWidget->commentsArea()->setFocus(Qt::ShortcutFocusReason);
	}
}

void ProjectManager::setFocusToErros() {

	if (m_errorTableHelper != NULL && m_errorTableHelper->errorTable() != NULL) {
		m_errorTableHelper->errorTable()->setFocus(Qt::ShortcutFocusReason);
	}
}

void ProjectManager::setFocusToProjectTree() {

	if (m_projectTreeHelper != NULL && m_projectTreeHelper->linkedTreeWidget() != NULL) {
		m_projectTreeHelper->linkedTreeWidget()->setFocus(Qt::ShortcutFocusReason);
	}
}