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
  m_project->saveProjectFile(projectFile);

  if (m_tabsHelper != NULL) {
    m_tabsHelper->addTabWithFile(projectFile);
  }
  if (m_projectTreeHelper != NULL) {
	  m_projectTreeHelper->update();
  }
}

void ProjectManager::removeProjectFile(SProjectFile projectFile) {

  m_project->removeProjectFile(projectFile);
  m_project->saveUpdatedStructure();
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
			this, SLOT(intemActivatedSlot(QTreeWidgetItem*)));
	}
}

SProjectFile ProjectManager::currentlyOpenProjectFile() {

	if (m_tabsHelper != NULL) {
		return m_tabsHelper->currentlyOpenProjectFile();
	}
	else {
		return SProjectFile(NULL);
	}	
}

void ProjectManager::saveAll() {

	if (m_project != NULL) {
		m_project->saveProject();
	}
}

void ProjectManager::saveCurrentlyOpenProjectFile() {

	if (m_project != NULL) {
		SProjectFile currentFile = currentlyOpenProjectFile();
		if (currentFile != NULL) {
			m_project->saveProjectFile(currentFile);
		}
	}
}

void ProjectManager::compile() {

	if (m_project != NULL) {
		QList<SProjectFile> files = m_project->projectFiles();
		QStringList *pathList = new QStringList();
		foreach (SProjectFile file, files) {
			pathList->append(file->path());
		}
		CompilerHelper *compileHelper = new CompilerHelper(pathList);
		compileHelper->compile();
	}
}

void ProjectManager::intemActivatedSlot(QTreeWidgetItem *item) {

	if (item != NULL) {
		QVariant data = item->data(0, Qt::UserRole);
		if (!data.isNull() && data.isValid()) {
			SProjectFile projectFile = data.value<SProjectFile>();
			m_tabsHelper->addTabWithFile(projectFile);
		}
	}
}
