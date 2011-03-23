#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>

#include "ProjectFile.h"

class Project {

    QString m_projectFilePath;
    QList<SProjectFile> m_projectFiles;

    void initEmptyProject();
    void loadFromFile();

	bool isFileAlreadyInProject(ProjectFile *projectFile);

public:
    Project(const QString &path);
    ~Project();

    bool saveProject();
    void addProjectFile(SProjectFile projectFile);
    void removeProjectFile(SProjectFile projectFile);

	QString path() const { return m_projectFilePath; }
	QList<SProjectFile> projectFiles() const { return m_projectFiles; }
};

#endif // PROJECT_H
