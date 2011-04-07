#pragma once

#include <QString>

#include "CompileTaskDelegate.h"
#include "CompileTask.h"

class CompileTaskFactory {

public:
	static CompileTask *makeTask(QString command, CompileTaskDelegate *delegate);
};