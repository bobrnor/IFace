#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>

#include "Project.h"
#include "ProjectFile.h"
#include "TabsHelper.h"
#include "ProjectTreeHelper.h"
#include "CompilerHelper.h"

class ProjectManager : QObject {
	Q_OBJECT

	Project *m_project;
	TabsHelper *m_tabsHelper;
	ProjectTreeHelper *m_projectTreeHelper;
	CompilerHelper *m_compileHelper;

	SProjectFile currentlyOpenProjectFile();

public:
	ProjectManager();
	ProjectManager(const QString &path);
	~ProjectManager();

	bool createProject(const QString &path);

	void addProjectFile(SProjectFile projectFile);
	void removeProjectFile(SProjectFile projectFile);

	void assignTabWidget(QTabWidget *tabWidget);
	void assignProjectTreeWidget(QTreeWidget *projectTree);

	void saveAll();
	void saveCurrentlyOpenProjectFile();

	void compile();

public slots:
	void intemActivatedSlot(QTreeWidgetItem *item);
	void compileCompleteSlot();
};

#endif // PROJECTMANAGER_H
