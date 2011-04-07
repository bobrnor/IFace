#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThreadPool>

#include "CompileTaskDelegate.h"

class CompilerHelper : public CompileTaskDelegate {
Q_OBJECT

	const QString COMPILER_PATH;	
	const QString COMPILER_FLAGS_RU;
	const QString COMPILER_FLAGS_EN;
	const QString COMPILER_PATTERN;

	QStringList *m_pathList;

private:

public:
	CompilerHelper(QStringList *pathList);
	~CompilerHelper();

	void compile();

public slots:
	void compileErrorSlot(QString command);
	void compileDoneSlot(QString command, CompileResult *result = NULL);
};