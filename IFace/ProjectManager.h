#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>

#include "Project.h"
#include "ProjectFile.h"
#include "TabsHelper.h"

class ProjectManager {

  Project *m_project;
  TabsHelper *m_tabsHelper;

public:
    ProjectManager();
    ProjectManager(const QString &path);
    ~ProjectManager();

    bool createProject(const QString &path);

    void addProjectFile(ProjectFile &projectFile);
    void removeProjectFile(const ProjectFile &projectFile);

    void assignTabWidget(QTabWidget *tabWidget);
};

#endif // PROJECTMANAGER_H
