#include "ProjectManager.h"

ProjectManager::ProjectManager() {

    m_project = NULL;
}

ProjectManager::ProjectManager(const QString &path) {

    if (path.length() > 0) {
        m_project = new Project(path);
    }
    else {
        ProjectManager::ProjectManager();
    }
}

ProjectManager::~ProjectManager() {

    if (m_project != NULL) {
        delete m_project;
    }
}

bool ProjectManager::createProject(const QString &path) {

    if (m_project == NULL && path.length() > 0) {
        m_project = new Project(path);
        return true;
    }
    return false;
}
