#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QSet>
#include <QString>
#include <QSharedPointer>

#define SProjectFile QSharedPointer<ProjectFile>

class ProjectFile {

  QString m_path;
  QSet<int> m_breakPoints;

  bool createIfNotExists();

public:
	ProjectFile();
    ProjectFile(const QString &path);
	virtual ~ProjectFile();

    QString path() const { return m_path; }
    void setPath(const QString &path);

	bool checkLineForBreakPoints(int lineNumber) const;
	void addBreakPoint(int lineNumber);
	void removeBreakPoint(int lineNumber);

    bool operator==(const ProjectFile &projectFile) const;
};

#endif // PROJECTFILE_H
