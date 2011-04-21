#include "ErrorParser.h"

// test_file_2(1,4) Fatal: Syntax error, "BEGIN" expected but "identifier SDF" found

CompileError ErrorParser::parse(QString string) {

	static QRegExp rx("^([^\\(\\)]*)\\((\\d+),(\\d+)\\) (.*)$");
	CompileError error;
	if (rx.indexIn(string) >= 0) {
		error.setIsValid(true);
		QStringList captures = rx.capturedTexts();		
		error.setYPos(captures.at(2).toInt());
		error.setXPos(captures.at(3).toInt());
		error.setText(captures.at(4));
		return error;
	}
	else {
		error.setIsValid(false);
		return error;
	}
}