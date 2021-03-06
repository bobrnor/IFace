#include "CompileTask.h"

#include <QDebug>
#include <QProcess>
#include <QList>
#include <QTextCodec>

#include "ErrorParser.h"

void CompileTask::run() {

	COMPILER_PATH = "C:\\FPC\\2.4.2\\bin\\i386-win32\\fpc.exe"; 
	COMPILER_FLAGS_EN = "";
	COMPILER_FLAGS_RU = "-Fr\"C:\\FPC\\2.4.2\\msg\\errorrw.msg\"";
	COMPILER_PATTERN = "%1 %2 \"%3\"";

	connect(this, SIGNAL(compileErrorSignal(uint)), 
		m_delegate, SLOT(compileErrorSlot(uint)));
	connect(this, SIGNAL(compileDoneSignal(uint)), 
		m_delegate, SLOT(compileDoneSlot(uint)));

	runEn();
	runRu();

	emit compileDoneSignal(m_key);
}

void CompileTask::runEn() {

	QString command = makeCommandForEn();

	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(command);
	proc.waitForFinished();

	QString result = proc.readAllStandardOutput();
	qDebug() << result;
	QStringList lines = result.split("\r\n", QString::SkipEmptyParts);
	QList<CompileError> errors;

	foreach (QString line, lines) {
		CompileError error = ErrorParser::parse(line);
		if (error.isValid()) {
			error.setProjectFile(m_projectFile);
			errors.append(error);
		}
	}
	m_projectFile->setCompileErrorsEn(errors);
}

void CompileTask::runRu() {

	QString command = makeCommandForRu();

	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(command);
	proc.waitForFinished();

	QString result = proc.readAllStandardOutput();
	qDebug() << result;
	QStringList lines = result.split("\r\n", QString::SkipEmptyParts);
	QList<CompileError> errors;

	foreach (QString line, lines) {
		CompileError error = ErrorParser::parse(line);
		if (error.isValid()) {
			error.setProjectFile(m_projectFile);
			errors.append(error);
		}
	}
	m_projectFile->setCompileErrorsRu(errors);
}

QString CompileTask::makeCommandForEn() {

	QString path = m_projectFile->tmpPath();
	if (path.isEmpty()) {
		path = m_projectFile->path();
	}
	QString command = QString(COMPILER_PATTERN)
		.arg(COMPILER_PATH)
		.arg(COMPILER_FLAGS_EN)
		.arg(path);

	return command;
}

QString CompileTask::makeCommandForRu() {

	QString path = m_projectFile->tmpPath();
	if (path.isEmpty()) {
		path = m_projectFile->path();
	}
	QString command = QString(COMPILER_PATTERN)
		.arg(COMPILER_PATH)
		.arg(COMPILER_FLAGS_RU)
		.arg(path);

	return command;
}