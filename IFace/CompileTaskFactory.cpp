#include "CompileTaskFactory.h"

CompileTask *CompileTaskFactory::makeTask(uint key, SProjectFile projectFile, CompileTaskDelegate *delegate) {

	CompileTask *task = new CompileTask();
	task->setKey(key);
	task->setProjectFile(projectFile);
	task->setDelegate(delegate);
	return task;
}