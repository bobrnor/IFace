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

	m_translator = new QTranslator;
	m_qtTranslator = new QTranslator;
	m_qtTranslator->load("qt_ru.qm", ".");

	m_lang = 0;

	LAST_PROJECTS_KEY = "last_projects";
	EN_HIGH_FREQUENCE_ERROR_KEY = "en_hfe";
	RU_HIGH_FREQUENCE_ERROR_KEY = "ru_hfe";
	LAST_LANG_KEY = "last_lang";

	m_textHighlightColor = Qt::green;
	m_enHighFrquenceError = "";
	m_ruHighFrquenceError = "";

	m_eventFilter = NULL;

	loadSettings();

	if (m_lang) {
		m_translator->load("iface_ru.qm", ".");
	}
	else {
		m_translator->load("iface_en.qm", ".");
	}
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
	settings.setValue(EN_HIGH_FREQUENCE_ERROR_KEY, m_enHighFrquenceError);
	settings.setValue(RU_HIGH_FREQUENCE_ERROR_KEY, m_ruHighFrquenceError);
	settings.setValue(LAST_LANG_KEY, m_lang);
	settings.sync();
}

void GlobalState::loadSettings() {

	QSettings settings("SelfSoft", "iFace");

	QVariantList pathList = settings.value(LAST_PROJECTS_KEY, QVariantList()).toList();
	foreach (QVariant path, pathList) {
		QString projectPath = path.toString();
		m_lastProjects.append(projectPath);
	}

	m_enHighFrquenceError = settings.value(EN_HIGH_FREQUENCE_ERROR_KEY, "").toString();
	m_ruHighFrquenceError = settings.value(RU_HIGH_FREQUENCE_ERROR_KEY, "").toString();
	m_lang = settings.value(LAST_LANG_KEY, 0).toInt();
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

void GlobalState::addErrors(QList<CompileError> enErrors, QList<CompileError>ruErrors) {

	int count = -1;
	QString errorPattern;

	foreach (CompileError error, enErrors) {
		QString pattern = error.errorPattern();
		if (m_enErrorFrenquence.contains(pattern)) {
			m_enErrorFrenquence[pattern]++;
		}
		else {
			m_enErrorFrenquence[pattern] = 0;
		}
		if (count < m_enErrorFrenquence[pattern]) {
			count = m_enErrorFrenquence[pattern];
			errorPattern = pattern;
		}
	}

	m_enHighFrquenceError = errorPattern;

	count = -1;
	errorPattern = "";

	foreach (CompileError error, ruErrors) {
		QString pattern = error.errorPattern();
		if (m_ruErrorFrenquence.contains(pattern)) {
			m_ruErrorFrenquence[pattern]++;
		}
		else {
			m_ruErrorFrenquence[pattern] = 0;
		}
		if (count < m_ruErrorFrenquence[pattern]) {
			count = m_ruErrorFrenquence[pattern];
			errorPattern = pattern;
		}
	}

	m_ruHighFrquenceError = errorPattern;
	saveSettings();
}