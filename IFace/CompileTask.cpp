#include "CompileTask.h"

#include <QDebug>
#include <QProcess>
#include <QList>

#include "ErrorParser.h"

void CompileTask::run() {

	prerunInit();
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(m_command);
	proc.waitForFinished();

	QString result = proc.readAllStandardOutput();
	QStringList lines = result.split("\n", QString::SkipEmptyParts);
	QList<CompileError> errors;

	foreach (QString line, lines) {
		CompileError error = ErrorParser::parse(line);
		if (error.m_isValid) {
			errors.append(error);
		}
	}
}

void CompileTask::prerunInit() {

	connect(this, SIGNAL(compileErrorSignal(QString)), 
		m_delegate, SLOT(compileErrorSlot(QString)));
	connect(this, SIGNAL(compileDoneSignal(QString, CompileResult*)), 
		m_delegate, SLOT(compileDoneSlot(QString, CompileResult*)));
}