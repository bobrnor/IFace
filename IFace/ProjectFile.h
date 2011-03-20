#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QList>
#include <QString>

class ProjectFile {

  QString m_path;
  QList<int> m_breakPoints;
  QMutex *mutex;

  bool createIfNotExists();

public:
    ProjectFile(const QString &path);
	virtual ~ProjectFile();

    QString path() const { return m_path; }
    void setPath(const QString &path) { m_path = path; }

	bool checkLineForBreakPoints(int lineNumber);
	void addBreakPoint(int lineNumber);
	void removeBreakPoint(int lineNumber);

    bool operator==(const ProjectFile &projectFile) const;
};

#endif // PROJECTFILE_H
