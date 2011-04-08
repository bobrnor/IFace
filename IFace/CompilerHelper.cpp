#include "CompilerHelper.h"

#include <QDebug>

#include "CompileTask.h"
#include "CompileTaskFactory.h"

CompilerHelper::CompilerHelper(QMap<uint, SProjectFile> filesToCompile) {

	m_filesToCompile = filesToCompile;
}

CompilerHelper::~CompilerHelper() {}

void CompilerHelper::compile() {

	QString command;
	QString path;
	CompileTask *task = NULL;

	m_fileCount = m_filesToCompile.count();

	foreach (uint key, m_filesToCompile.keys()) {
		
		SProjectFile file = m_filesToCompile.value(key);
		task = CompileTaskFactory::makeTask(key, file, this);
		QThreadPool::globalInstance()->start(task);
	}
}

void CompilerHelper::compileErrorSlot(uint key) {

	qDebug() << "Compile error: " << key;
}

void CompilerHelper::compileDoneSlot(uint key) {

	qDebug() << "Compiled: " << key;
	m_fileCount--;
	if (m_fileCount == 0) {
		emit compileCompleteSignal();
	}
}