#pragma once

#include <QTreeWidget>
#include "Project.h"

class ProjectTreeHelper {

	QTreeWidget *m_linkedTreeWidget;
	Project *m_project;

protected:

public:
	ProjectTreeHelper(QTreeWidget *projectTree);
	~ProjectTreeHelper();

	void setProject(Project *project);
	Project *project() const { return m_project; }

	QTreeWidget *linkedTreeWidget() const { return m_linkedTreeWidget; }

	void update();
};