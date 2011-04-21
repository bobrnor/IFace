#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThreadPool>

#include "CompileTaskDelegate.h"
#include "ProjectFile.h"

class CompilerHelper : public CompileTaskDelegate {
Q_OBJECT

	int m_fileCount;
	QMap<uint, ProjectFile *> m_filesToCompile;

private:

public:
	CompilerHelper(QMap<uint, ProjectFile *> filesToCompile);
	~CompilerHelper();

	void compile();

public slots:
	void compileErrorSlot(uint key);
	void compileDoneSlot(uint key);

signals:
	void compileCompleteSignal();
};