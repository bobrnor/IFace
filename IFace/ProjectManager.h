#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>

#include "Project.h"
#include "ProjectFile.h"
#include "TabsHelper.h"
#include "ProjectTreeHelper.h"

class ProjectManager : QObject {
	Q_OBJECT

	Project *m_project;
	TabsHelper *m_tabsHelper;
	ProjectTreeHelper *m_projectTreeHelper;

public:
	ProjectManager();
	ProjectManager(const QString &path);
	~ProjectManager();

	bool createProject(const QString &path);

	void addProjectFile(SProjectFile projectFile);
	void removeProjectFile(SProjectFile projectFile);

	void assignTabWidget(QTabWidget *tabWidget);
	void assignProjectTreeWidget(QTreeWidget *projectTree);

	public slots:
		void onTreeItemDoubleClickedSlot(QTreeWidgetItem *item);
};

#endif // PROJECTMANAGER_H
