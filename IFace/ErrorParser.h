#pragma once

#include "CompileError.h"

#include <QRegExp>
#include <QString>
#include <QStringList>

class ErrorParser {

public:
	static CompileError parse(QString string);
};