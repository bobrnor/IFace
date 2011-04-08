#pragma once

#include <QRunnable>

#include "CompileTaskDelegate.h"
#include "ProjectFile.h"

class CompileTask : public QObject, public QRunnable {
Q_OBJECT

	QString COMPILER_PATH;	
	QString COMPILER_FLAGS_RU;
	QString COMPILER_FLAGS_EN;
	QString COMPILER_PATTERN;

	uint m_key;
	SProjectFile m_projectFile;
	CompileTaskDelegate *m_delegate;

	void runEn();
	void runRu();
	QString makeCommandForEn();
	QString makeCommandForRu();

public:
	void run();

	uint key() { return m_key; }
	void setKey(uint _key) { m_key = _key; }
	SProjectFile projectFile() { return m_projectFile; }
	void setProjectFile(SProjectFile _projectFile) { m_projectFile = _projectFile; }
	CompileTaskDelegate *delegate() { return m_delegate; }
	void setDelegate(CompileTaskDelegate *_delegate) { m_delegate = _delegate; }

signals:
	void compileErrorSignal(uint key);
	void compileDoneSignal(uint key);

};