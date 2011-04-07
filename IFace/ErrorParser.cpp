#include "ErrorParser.h"

// test_file_2(1,4) Fatal: Syntax error, "BEGIN" expected but "identifier SDF" found

CompileError ErrorParser::parse(QString string) {

	static QRegExp rx("^([^\\(\\)]*)\\((\\d+),(\\d+)\\) (.*)$");
	CompileError error;
	if (rx.indexIn(string) >= 0) {
		error.m_isValid = true;
		QStringList captures = rx.capturedTexts();
		error.m_fileName = captures.at(1);
		error.m_xPos = captures.at(2).toInt();
		error.m_yPos = captures.at(3).toInt();
		error.m_text = captures.at(4);
		return error;
	}
	else {
		error.m_isValid = false;
		return error;
	}
}