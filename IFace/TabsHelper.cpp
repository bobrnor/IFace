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
		widget->setCodeEditor(codeEditor);
		CommentsEditor *commentsArea = new CommentsEditor(widget);
		commentsArea->setProjectFile(file);
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