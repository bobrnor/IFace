#pragma once

#include <QString>
#include <QList>
#include <QColor>
#include <QTranslator>

#include "Project.h"
#include "CompileError.h"

class GlobalState {

	static GlobalState *m_instance;
	QString LAST_PROJECTS_KEY;
	QString EN_HIGH_FREQUENCE_ERROR_KEY;
	QString RU_HIGH_FREQUENCE_ERROR_KEY;
	QString LAST_LANG_KEY;

	QList<QString> m_lastProjects;
	QColor m_textHighlightColor;

	QString m_enHighFrquenceError;
	QString m_ruHighFrquenceError;
	QMap<QString, int> m_enErrorFrenquence;
	QMap<QString, int> m_ruErrorFrenquence;

	int m_lang;
	QTranslator *m_translator;
	QTranslator *m_qtTranslator;

	QObject *m_eventFilter;

	GlobalState();
	void loadSettings();

public:
	static GlobalState *instance();
	~GlobalState();

	void saveSettings();

	QObject *eventFilter() { return m_eventFilter; }
	void setEventFilter(QObject *eventFilter) { m_eventFilter = eventFilter; }

	int langId() { return m_lang; }
	void setLangId(int langId) { m_lang = langId; }

	QTranslator *translator() { return m_translator; }
	QTranslator *qtTranslator() { return m_qtTranslator; }

	void projectOpened(Project *project);
	QList<QString> lastProjects() { return m_lastProjects; }

	void setTextHighlightColor(QColor color) { m_textHighlightColor = color; }
	QColor textHighlightColor() { return m_textHighlightColor; }

	void addErrors(QList<CompileError> enErrors, QList<CompileError>ruErrors);
	QString enHighFrequenceError() { return m_enHighFrquenceError; }
	QString ruHighFrequenceError() { return m_ruHighFrquenceError; }
};