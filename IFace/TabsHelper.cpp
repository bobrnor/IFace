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

void TabsHelper::addTabWithFile(SProjectFile file) {

	
	Q_ASSERT(m_tabWidget != NULL);

	if (!isFileAlreadyOpen(file.data())) {
		CodeEditorWidget *widget = new CodeEditorWidget(m_tabWidget);

		CodeEditor *codeEditor = new CodeEditor(widget);
		codeEditor->setProjectFile(file);		

		CommentsEditor *commentsArea = new CommentsEditor(widget);		
		commentsArea->setProjectFile(file);

		widget->setCodeEditor(codeEditor);
		widget->setCommentsArea(commentsArea);		

		m_tabWidget->addTab(widget, file->path());
		m_openFiles.append(file);
	}
}

bool TabsHelper::isFileAlreadyOpen(ProjectFile *file) const {

	
	bool result = false;
	foreach (SProjectFile sfile, m_openFiles) {
		if (sfile->path() == file->path()) {
			result = true;
			break;
		}
	}
	return result;
}

SProjectFile TabsHelper::currentlyOpenProjectFile() {

	if (m_tabWidget != NULL) {
		QWidget *curentWidget = m_tabWidget->currentWidget();
		if (curentWidget != NULL) {
			CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(curentWidget);
			SProjectFile projectFile = codeEditorWidget->codeEditor()->projectFile();
			return projectFile;
		}
	}
	return SProjectFile(NULL);
}

QMap<uint, SProjectFile> TabsHelper::tempPathsForOpenFiles() {

	QMap<uint, SProjectFile> result;

	if (m_tabWidget != NULL) {
		int widgetCount = m_tabWidget->count();
		for (int i = 0; i < widgetCount; ++i) {
			QWidget *widget = m_tabWidget->widget(i);
			if (widget != NULL) {
				CodeEditorWidget codeEditorWidget = static_cast<CodeEditorWidget *>(widget);
				SProjectFile projectFile = codeEditorWidget.codeEditor()->projectFile();
				result[projectFile->hash()] = projectFile;
			}
		}
	}

	return result;
}