#include "CompileError.h"

#include <QRegExp>

QString CompileError::errorPattern() {

	static QRegExp rx("\"(.*)\"");

	QString errorPattern = m_text.replace(rx, "");
	return errorPattern;
}