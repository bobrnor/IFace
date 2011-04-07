#include "CompilerHelper.h"

#include "CompileTask.h"
#include "CompileTaskFactory.h"

CompilerHelper::CompilerHelper(QStringList *pathList) : 
	COMPILER_PATH("C:\\FPC\\2.4.2\\bin\\i386-win32\\fpc.exe"), 
	COMPILER_FLAGS_EN("-Fr\"C:\\FPC\\2.4.2\\msg\\errorrw.msg\""), 
	COMPILER_FLAGS_RU(""), 
	COMPILER_PATTERN("%1 %2 \"%3\"") {

	m_pathList = pathList;
}

CompilerHelper::~CompilerHelper() {

	if (m_pathList != NULL) {
		delete m_pathList;
	}
}

void CompilerHelper::compile() {

	if (m_pathList != NULL) {
		QString command;
		CompileTask *task = NULL;

		foreach (QString path, *m_pathList) {
			command = QString(COMPILER_PATTERN)
				.arg(COMPILER_PATH)
				.arg(COMPILER_FLAGS_EN)
				.arg(path);
			task = CompileTaskFactory::makeTask(command, this);
			QThreadPool::globalInstance()->start(task);

			command = QString(COMPILER_PATTERN)
				.arg(COMPILER_PATH)
				.arg(COMPILER_FLAGS_RU)
				.arg(path);
			task = CompileTaskFactory::makeTask(command, this);
			QThreadPool::globalInstance()->start(task);
		}
	}
}

void CompilerHelper::compileErrorSlot(QString command) {

}

void CompilerHelper::compileDoneSlot(QString command, CompileResult *result) {

}