#include "ProjectFile.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include "CodeEditor.h"

ProjectFile::ProjectFile() {}

ProjectFile::ProjectFile(const QString &path) {

	m_path = path;
	createIfNotExists();
}

ProjectFile::~ProjectFile() {

	qDebug() << __FUNCSIG__;
}

void ProjectFile::setPath(const QString &path) {

	m_path = path;
	createIfNotExists();
}

QString ProjectFile::fileName() const {

	if (!m_path.isEmpty()) {
		QFileInfo pathInfo(m_path);
		return pathInfo.fileName();
	}
	return "";
}

void ProjectFile::setTmpPath(const QString &path) {

	m_tmpPath = path;
}

void ProjectFile::linkCodeEditor(CodeEditor *codeEditor) {

	m_linkedCodeEditor = codeEditor;
}

void ProjectFile::save() {

	if (m_linkedCodeEditor != NULL) {
		m_linkedCodeEditor->saveProjectFile();
	}
}

bool ProjectFile::createIfNotExists() {

	
	if (!QFile::exists(m_path)) {
		QFile file(m_path);
		bool ok = file.open(QIODevice::WriteOnly);

		Q_ASSERT(ok);

		if (ok) {
			file.close();
		}

		return ok;
	}
	return true;
}

bool ProjectFile::operator==(const ProjectFile &projectFile) const {

	
	return m_path == projectFile.path();
}

bool ProjectFile::checkLineForBreakPoints(int lineNumber) const {

	
	return m_breakPoints.contains(lineNumber);
}

void ProjectFile::addBreakPoint(int lineNumber) {

	
	m_breakPoints.insert(lineNumber);
}

void ProjectFile::removeBreakPointIfExists(int lineNumber) {

	
	if (m_breakPoints.contains(lineNumber)) {
		m_breakPoints.remove(lineNumber);
	}	
}

QString ProjectFile::commentInLine(int lineNumber) {

	
	if (m_comments.contains(lineNumber)) {
		return m_comments.value(lineNumber);
	}
	else {
		return QString("");
	}
}

void ProjectFile::addOrReplaceComment(int lineNumber, QString comment) {

	
	m_comments[lineNumber] = comment;
}

void ProjectFile::removeCommentIfExists(int lineNumber) {

	if (m_comments.contains(lineNumber)) {
		m_comments.remove(lineNumber);
	}
}