#include "CompileError.h"

#include <QRegExp>

QString CompileError::errorPattern() {

	static QRegExp rx("\"([^\"]*)\"");

	QString errorPattern = QString(m_text).replace(rx, "");
	return errorPattern;
}