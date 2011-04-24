#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>
#include <QTableView>

#include "Project.h"
#include "ProjectFile.h"
#include "TabsHelper.h"
#include "ProjectTreeHelper.h"
#include "CompilerHelper.h"
#include "ErrorTableHelper.h"

class ProjectManager : QObject {
	Q_OBJECT

	Project *m_project;
	ErrorTableHelper *m_errorTableHelper;
	TabsHelper *m_tabsHelper;
	ProjectTreeHelper *m_projectTreeHelper;
	CompilerHelper *m_compileHelper;

	ProjectFile *currentlyOpenProjectFile();

public:
	ProjectManager();
	ProjectManager(const QString &path);
	~ProjectManager();

	bool createProject(const QString &path);
	Project *project() { return m_project; }

	void addProjectFile(ProjectFile *projectFile);
	void removeProjectFile(ProjectFile *projectFile);

	void assignTabWidget(QTabWidget *tabWidget);
	void assignProjectTreeWidget(QTreeWidget *projectTree);
	void assignErrorWidget(QTableView *errorWidget);

	void saveAll();
	void saveCurrentlyOpenProjectFile();

	void compile();

	bool beginCloseProject();
	void endCloseProject();

	void setFocusToCode();
	void setFocusToComments();
	void setFocusToErros();
	void setFocusToProjectTree();

public slots:
	void intemActivatedSlot(QTreeWidgetItem *item);
	void compileCompleteSlot();
	void errorPositionChangedSlot(ProjectFile *file, int xPos, int yPos);
};

#endif // PROJECTMANAGER_H
