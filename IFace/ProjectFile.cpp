#include "ProjectFile.h"

#include <QDebug>
#include <QFile>

ProjectFile::ProjectFile() {}

ProjectFile::ProjectFile(const QString &path) {

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
	return m_path == projectFile.path();
}

bool ProjectFile::checkLineForBreakPoints(int lineNumber) const {

	qDebug() << __FUNCSIG__;
	return m_breakPoints.contains(lineNumber);
}

void ProjectFile::addBreakPoint(int lineNumber) {

	qDebug() << __FUNCSIG__;
	m_breakPoints.insert(lineNumber);
}

void ProjectFile::removeBreakPoint(int lineNumber) {

	qDebug() << __FUNCSIG__;
	m_breakPoints.remove(lineNumber);
}
