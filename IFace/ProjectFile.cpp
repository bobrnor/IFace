#include "ProjectFile.h"

#include <QDebug>
#include <QFile>

ProjectFile::ProjectFile(const QString &path) {

  m_path = path;
  createIfNotExists();
}

ProjectFile::~ProjectFile() {

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

bool ProjectFile::checkLineForBreakPoints(int lineNumber) {

	return m_breakPoints.count() > 0 && m_breakPoints.contains(lineNumber);
}

void ProjectFile::addBreakPoint(int lineNumber) {

	if (!checkLineForBreakPoints(lineNumber)) {
		m_breakPoints.append(lineNumber);
	}	
}

void ProjectFile::removeBreakPoint(int lineNumber) {

	m_breakPoints.removeAll(lineNumber);
}
