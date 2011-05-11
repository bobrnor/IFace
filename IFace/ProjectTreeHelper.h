#pragma once

#include <QTreeWidget>
#include "Project.h"

class ProjectTreeHelper : public QObject {
Q_OBJECT

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

signals:
	void addNewFileToProjectSignal();
	void addExistingFileToProjectSignal();
	void removeFileFromProjectSignal(ProjectFile *file);

private slots:
	void contextMenuRequestSlot(const QPoint &pos);
	void contextMenuRemoveFileSlot();

};