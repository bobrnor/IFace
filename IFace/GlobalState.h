#pragma once

#include <QString>
#include <QList>

#include "Project.h"

class GlobalState {

	static GlobalState *m_instance;
	QString LAST_PROJECTS_KEY;

	QList<QString> m_lastProjects;

	GlobalState();
	void loadSettings();

public:
	static GlobalState *instance();
	~GlobalState();

	void saveSettings();

	void projectOpened(Project *project);
	QList<QString> lastProjects() { return m_lastProjects; }
};