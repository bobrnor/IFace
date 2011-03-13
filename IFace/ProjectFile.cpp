#include "ProjectFile.h"

#include <QFile>

ProjectFile::ProjectFile(const QString &path) {

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
