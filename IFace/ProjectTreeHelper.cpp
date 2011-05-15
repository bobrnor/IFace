#include "ProjectTreeHelper.h"

#include <QMenu>

#include "Project.h"
#include "ProjectFile.h"

Q_DECLARE_METATYPE(ProjectFile *)

ProjectTreeHelper::ProjectTreeHelper(QTreeWidget *projectTree) : m_linkedTreeWidget(projectTree) {

	m_project = NULL;
	m_linkedTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_linkedTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), 
		this, SLOT(contextMenuRequestSlot(const QPoint &)));
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
			item->setText(0, m_project->projectName());
			item->setToolTip(0, m_project->path());
			foreach (ProjectFile *file, m_project->projectFiles()) {
				QTreeWidgetItem *child = new QTreeWidgetItem();
				child->setData(0, Qt::UserRole, qVariantFromValue(file));
				child->setText(0, file->fileName());
				child->setToolTip(0, file->path());
				item->addChild(child);
			}

			m_linkedTreeWidget->addTopLevelItem(item);
		}
	}
}

void ProjectTreeHelper::contextMenuRequestSlot(const QPoint &pos) {

	QMenu *menu = new QMenu(m_linkedTreeWidget);
	menu->addAction(tr("Add New File..."), this, SIGNAL(addNewFileToProjectSignal()));
	menu->addAction(tr("Add Existing File..."), this, SIGNAL(addExistingFileToProjectSignal()));
	QAction *removeAction = menu->addAction(tr("Remove File"), this, SLOT(contextMenuRemoveFileSlot()));

	QTreeWidgetItem *currentItem = m_linkedTreeWidget->currentItem();
	if (currentItem != NULL && currentItem->data(0, Qt::UserRole).value<ProjectFile *>() != NULL) {
		removeAction->setEnabled(true);
	}
	else {
		removeAction->setEnabled(false);
	}

	menu->exec(m_linkedTreeWidget->mapToGlobal(pos));
}

void ProjectTreeHelper::contextMenuRemoveFileSlot() {

	QTreeWidgetItem *currentItem = m_linkedTreeWidget->currentItem();
	if (currentItem != NULL && currentItem->data(0, Qt::UserRole).value<ProjectFile *>() != NULL) {
		ProjectFile *file = currentItem->data(0, Qt::UserRole).value<ProjectFile *>();
		emit removeFileFromProjectSignal(file);
	}
}