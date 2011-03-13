#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>

#include "ProjectFile.h"

class Project {

    QString m_projectFilePath;
    QList<ProjectFile> m_projectFiles;

    void initEmptyProject();
    void loadFromFile();

public:
    Project(const QString &path);
    ~Project();

    bool saveProject();
    void addProjectFile(const ProjectFile &projectFile);
    void removeProjectFile(const ProjectFile &projectFile);
};

#endif // PROJECT_H
