#include "ProjectFile.h"

#include <QDebug>
#include <QFile>

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
		return m_comments.take(lineNumber);
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