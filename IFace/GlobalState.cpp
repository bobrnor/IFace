#include "GlobalState.h"

#include <QSettings>
#include <QVariant>
#include <QVariantList>
#include <QStringList>

#include <QDebug>

GlobalState *GlobalState::m_instance = new GlobalState();

GlobalState *GlobalState::instance() {

	return m_instance;
}

GlobalState::GlobalState() {

	LAST_PROJECTS_KEY = "last_projects";
	m_textHighlightColor = Qt::green;

	loadSettings();
}

GlobalState::~GlobalState() {

}

void GlobalState::saveSettings() {

	QSettings settings("SelfSoft", "iFace");
	QVariantList pathList;
	foreach(QString path, m_lastProjects) {
		pathList.append(QVariant(path));
		if (pathList.count() > 5) {
			break;
		}
	}
	settings.setValue(LAST_PROJECTS_KEY, pathList);
	settings.sync();
}

void GlobalState::loadSettings() {

	QSettings settings("SelfSoft", "iFace");

	QVariantList pathList = settings.value(LAST_PROJECTS_KEY, QVariantList()).toList();
	foreach (QVariant path, pathList) {
		QString projectPath = path.toString();
		m_lastProjects.append(projectPath);
	}
}

void GlobalState::projectOpened(Project *project) {

	if (project != NULL) {
		QString projectPath = project->path();
		if (!projectPath.isEmpty()) {
			m_lastProjects.removeAll(projectPath);
			m_lastProjects.push_front(projectPath);
		}
		saveSettings();
	}
}