#include "TabsHelper.h"

#include <QDebug>

#include "CodeEditor.h"
#include "CommentsEditor.h"
#include "CodeEditorWidget.h"

TabsHelper::TabsHelper() {

	
	m_tabWidget = NULL;
}

TabsHelper::TabsHelper(QTabWidget *tabWidget) {

	qDebug() << __FUNCSIG__;
	m_tabWidget = tabWidget;
}

void TabsHelper::showTabWithFile(ProjectFile *file) {

	
	Q_ASSERT(m_tabWidget != NULL);

	int tabNumber = -1;

	if (!isFileAlreadyOpen(file)) {
		CodeEditorWidget *widget = new CodeEditorWidget(m_tabWidget);

		CodeEditor *codeEditor = new CodeEditor(widget);
		codeEditor->setProjectFile(file);		

		connect(codeEditor, SIGNAL(codeCursorChangedSignal(ProjectFile*, int, int)), 
			this, SIGNAL(codeCursorChangedSignal(ProjectFile*, int, int)));

		CommentsEditor *commentsArea = new CommentsEditor(widget);		
		commentsArea->setProjectFile(file);

		widget->setCodeEditor(codeEditor);
		widget->setCommentsArea(commentsArea);		

		tabNumber = m_tabWidget->addTab(widget, file->path());
		m_openFiles.append(file);
	}
	else {
		if (m_tabWidget != NULL) {
			int widgetCount = m_tabWidget->count();
			for (int i = 0; i < widgetCount; ++i) {
				QWidget *widget = m_tabWidget->widget(i);
				if (widget != NULL) {
					CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(widget);
					CodeEditor *codeEditor = codeEditorWidget->codeEditor();
					if (codeEditor->projectFile()->path() == file->path()) {
						tabNumber = i;
						break;
					}
				}
			}
		}
	}
	if (tabNumber >= 0) {
		m_tabWidget->setCurrentIndex(tabNumber);
	}
}

bool TabsHelper::isFileAlreadyOpen(ProjectFile *file) const {

	bool result = false;
	foreach (ProjectFile *sfile, m_openFiles) {
		if (sfile->path() == file->path()) {
			result = true;
			break;
		}
	}
	return result;
}

ProjectFile *TabsHelper::currentlyOpenProjectFile() {

	if (m_tabWidget != NULL) {
		QWidget *curentWidget = m_tabWidget->currentWidget();
		if (curentWidget != NULL) {
			CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(curentWidget);
			ProjectFile *projectFile = codeEditorWidget->codeEditor()->projectFile();
			return projectFile;
		}
	}
	return NULL;
}

QMap<uint, ProjectFile *> TabsHelper::openFiles() {

	QMap<uint, ProjectFile *> result;

	if (m_tabWidget != NULL) {
		int widgetCount = m_tabWidget->count();
		for (int i = 0; i < widgetCount; ++i) {
			QWidget *widget = m_tabWidget->widget(i);
			if (widget != NULL) {
				CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(widget);
				CodeEditor *codeEditor = codeEditorWidget->codeEditor();
				codeEditor->tempSaveProjectFile();
				ProjectFile *projectFile = codeEditor->projectFile();
				result[projectFile->hash()] = projectFile;
			}
		}
	}

	return result;
}