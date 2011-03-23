#include "ProjectTreeHelper.h"

#include "Project.h"
#include "ProjectFile.h"

Q_DECLARE_METATYPE(SProjectFile)

ProjectTreeHelper::ProjectTreeHelper(QTreeWidget *projectTree) : m_linkedTreeWidget(projectTree) {

	m_project = NULL;
}

ProjectTreeHelper::~ProjectTreeHelper() {}

void ProjectTreeHelper::setProject(Project *project) {

	m_project = project;
	update();
}

void ProjectTreeHelper::update() {

	if (m_linkedTreeWidget != NULL) {
		m_linkedTreeWidget->clear();

		if (m_project != NULL) {
			QTreeWidgetItem *item = new QTreeWidgetItem();			
			item->setText(0, m_project->path());
			foreach (SProjectFile file, m_project->projectFiles()) {
				QTreeWidgetItem *child = new QTreeWidgetItem();
				child->setData(0, Qt::UserRole, qVariantFromValue(file));
				child->setText(0, file->path());
				item->addChild(child);
			}

			m_linkedTreeWidget->addTopLevelItem(item);
		}
	}
}