#include "TabsHelper.h"

TabsHelper::TabsHelper() {

  m_tabWidget = NULL;
}

TabsHelper::TabsHelper(QTabWidget *tabWidget) {

  m_tabWidget = tabWidget;
}

void TabsHelper::addTabWithFile(ProjectFile &file) {

  Q_ASSERT(m_tabWidget != NULL);

  CodeEditor *codeEditor = new CodeEditor(m_tabWidget);
  codeEditor->setProjectFile(&file);

  m_tabWidget->addTab(codeEditor, file.path());
}
