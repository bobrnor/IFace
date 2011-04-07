#include "CompileTaskFactory.h"

CompileTask *CompileTaskFactory::makeTask(QString command, CompileTaskDelegate *delegate) {

	CompileTask *task = new CompileTask();
	task->setCommand(command);
	task->setDelegate(delegate);
	return task;
}