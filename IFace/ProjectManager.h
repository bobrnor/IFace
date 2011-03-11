#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>

#include "Project.h"

class ProjectManager {

Project *m_project;

public:
    ProjectManager();
    ProjectManager(const QString &path);
    ~ProjectManager();

    bool createProject(const QString &path);
};

#endif // PROJECTMANAGER_H
