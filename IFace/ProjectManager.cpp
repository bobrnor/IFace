#include "ProjectManager.h"

Q_DECLARE_METATYPE(SProjectFile)

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

void ProjectManager::addProjectFile(SProjectFile projectFile) {

  m_project->addProjectFile(projectFile);
  m_project->saveProject();

  if (m_tabsHelper != NULL) {
    m_tabsHelper->addTabWithFile(projectFile);
  }
  if (m_projectTreeHelper != NULL) {
	  m_projectTreeHelper->update();
  }
}

void ProjectManager::removeProjectFile(SProjectFile projectFile) {

  m_project->removeProjectFile(projectFile);
  m_project->saveProject();
  if (m_projectTreeHelper != NULL) {
	  m_projectTreeHelper->update();
  }
}

void ProjectManager::assignTabWidget(QTabWidget *tabWidget) {

  if (tabWidget != NULL) {
    m_tabsHelper = new TabsHelper(tabWidget);
  }
}

void ProjectManager::assignProjectTreeWidget(QTreeWidget *projectTree) {

	if (projectTree != NULL) {
		m_projectTreeHelper = new ProjectTreeHelper(projectTree);
		m_projectTreeHelper->setProject(m_project);
		connect(m_projectTreeHelper->linkedTreeWidget(), SIGNAL(itemActivated(QTreeWidgetItem*, int)), 
			this, SLOT(onTreeItemDoubleClickedSlot(QTreeWidgetItem*)));
	}
}

void ProjectManager::onTreeItemDoubleClickedSlot(QTreeWidgetItem *item) {

	if (item != NULL) {
		QVariant data = item->data(0, Qt::UserRole);
		if (!data.isNull() && data.isValid()) {
			SProjectFile projectFile = data.value<SProjectFile>();
			m_tabsHelper->addTabWithFile(projectFile);
		}		
	}
}
