#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QSet>
#include <QMap>
#include <QString>
#include <QList>
#include <QSharedPointer>

#include "CompileError.h"

class CodeEditor;

//#define SProjectFile QSharedPointer<ProjectFile>

class ProjectFile {

  QString m_path;
  QString m_tmpPath;
  QSet<int> m_breakPoints;
  QMap<int, QString> m_comments;
  QList<CompileError> m_enErrors;
  QList<CompileError> m_ruErrors;

  CodeEditor *m_linkedCodeEditor;

  bool createIfNotExists();

public:
	ProjectFile();
    ProjectFile(const QString &path);
	virtual ~ProjectFile();

	uint hash() { return qHash(m_path); }

    QString path() const { return m_path; }
    void setPath(const QString &path);

	QString fileName() const;

	QString tmpPath() const { return m_tmpPath; }
	void setTmpPath(const QString &path);

	QList<CompileError> compileErrorsEn() { return m_enErrors; }
	void setCompileErrorsEn(QList<CompileError> compileErrors) { m_enErrors = compileErrors; }
	QList<CompileError> compileErrorsRu() { return m_ruErrors; }
	void setCompileErrorsRu(QList<CompileError> compileErrors) { m_ruErrors = compileErrors; }

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
