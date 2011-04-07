#pragma once

#include <QRunnable>

#include "CompileTaskDelegate.h"

class CompileTask : public QObject, public QRunnable {
Q_OBJECT

	QString m_command;
	CompileTaskDelegate *m_delegate;

	void prerunInit();

public:
	void run();

	QString command() { return m_command; }
	void setCommand(QString &cmd) { m_command = cmd; }
	CompileTaskDelegate *delegate() { return m_delegate; }
	void setDelegate(CompileTaskDelegate *_delegate) { m_delegate = _delegate; }

signals:
	void compileErrorSignal(QString m_command);
	void compileDoneSignal(QString m_command, CompileResult *result);

};