#include "TabsHelper.h"

#include "CodeEditor.h"
#include "CommentsArea.h"
#include "CodeEditorWidget.h"

TabsHelper::TabsHelper() {

  m_tabWidget = NULL;
}

TabsHelper::TabsHelper(QTabWidget *tabWidget) {

  m_tabWidget = tabWidget;
}

void TabsHelper::addTabWithFile(ProjectFile &file) {

  Q_ASSERT(m_tabWidget != NULL);

  CodeEditorWidget *widget = new CodeEditorWidget(m_tabWidget);

  CodeEditor *codeEditor = new CodeEditor(widget);
  codeEditor->setProjectFile(&file);
  widget->setCodeEditor(codeEditor);
  CommentsArea *commentsArea = new CommentsArea(widget);
  widget->setCommentsArea(commentsArea);

  m_tabWidget->addTab(widget, file.path());
}
