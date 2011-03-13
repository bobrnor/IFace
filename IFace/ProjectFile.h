#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QString>

class ProjectFile {

  QString m_path;

  bool createIfNotExists();

public:
    ProjectFile(const QString &path);

    QString path() const { return m_path; }
    void setPath(const QString &path) { m_path = path; }

    bool operator==(const ProjectFile &projectFile) const;
};

#endif // PROJECTFILE_H
