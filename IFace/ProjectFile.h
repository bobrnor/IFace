#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QSet>
#include <QMap>
#include <QString>
#include <QSharedPointer>

class CodeEditor;

#define SProjectFile QSharedPointer<ProjectFile>

class ProjectFile {

  QString m_path;
  QSet<int> m_breakPoints;
  QMap<int, QString> m_comments;

  CodeEditor *m_linkedCodeEditor;

  bool createIfNotExists();

public:
	ProjectFile();
    ProjectFile(const QString &path);
	virtual ~ProjectFile();

    QString path() const { return m_path; }
    void setPath(const QString &path);

	void linkCodeEditor(CodeEditor *codeEditor);
	void save();

	bool checkLineForBreakPoints(int lineNumber) const;
	void addBreakPoint(int lineNumber);
	void removeBreakPointIfExists(int lineNumber);

	QString commentInLine(int lineNumber);
	void addOrReplaceComment(int lineNumber, QString comment);
	void removeCommentIfExists(int lineNumber);

	QSet<int> breakPoints() const { return m_breakPoints; }
	QMap<int, QString> comments() const { return m_comments; }

    bool operator==(const ProjectFile &projectFile) const;
};

#endif // PROJECTFILE_H
