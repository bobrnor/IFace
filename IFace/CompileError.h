#pragma once

#include <QString>

class CompileError {

public:
	QString m_text;
	QString m_path;
	QString m_fileName;
	int m_xPos;
	int m_yPos;

	bool m_isValid;
};