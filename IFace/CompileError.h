#pragma once

#include <QString>

class ProjectFile;

class CompileError {

	ProjectFile *m_file;
	QString m_text;
	int m_xPos;
	int m_yPos;

	bool m_isValid;

public:
	ProjectFile *projectFile() { return m_file; }
	void setProjectFile(ProjectFile *file) { m_file = file; }

	QString text() { return m_text; }
	void setText(QString value) { m_text = value; }

	int xPos() { return m_xPos; }
	void setXPos(int value) { m_xPos = value; }

	int yPos() { return m_yPos; }
	void setYPos(int value) { m_yPos = value; }

	bool isValid() { return m_isValid; }
	void setIsValid(bool value) { m_isValid = value; }

	QString errorPattern();
};