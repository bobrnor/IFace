#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>
#include <QTableView>

#include "Project.h"
#include "ProjectFile.h"
#include "TabsHelper.h"
#include "ProjectTreeHelper.h"
#include "CompilerHelper.h"
#include <ErrorTableModel.h>

class ProjectManager : QObject {
	Q_OBJECT

	Project *m_project;
	QTableView *m_errorTable;
	ErrorTableModel *m_errorModel;
	TabsHelper *m_tabsHelper;
	ProjectTreeHelper *m_projectTreeHelper;
	CompilerHelper *m_compileHelper;

	SProjectFile currentlyOpenProjectFile();

public:
	ProjectManager();
	ProjectManager(const QString &path);
	~ProjectManager();

	bool createProject(const QString &path);

	QTableView *errorView() { return m_errorTable; }
	void setErrorView(QTableView *view);

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
