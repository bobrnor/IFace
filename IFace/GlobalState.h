#pragma once

#include <QString>
#include <QList>
#include <QColor>

#include "Project.h"

class GlobalState {

	static GlobalState *m_instance;
	QString LAST_PROJECTS_KEY;

	QList<QString> m_lastProjects;
	QColor m_textHighlightColor;

	GlobalState();
	void loadSettings();

public:
	static GlobalState *instance();
	~GlobalState();

	void saveSettings();

	void projectOpened(Project *project);
	QList<QString> lastProjects() { return m_lastProjects; }

	void setTextHighlightColor(QColor color) { m_textHighlightColor = color; }
	QColor textHighlightColor() { return m_textHighlightColor; }
};