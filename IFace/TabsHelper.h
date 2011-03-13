#ifndef TABSHELPER_H
#define TABSHELPER_H

#include <QTabWidget>
#include <QList>

#include "CodeEditor.h"
#include "ProjectFile.h"

class ProjectFile;

class TabsHelper {

  QTabWidget *m_tabWidget;
  QList<ProjectFile> m_openFiles;

public:
    TabsHelper();
    TabsHelper(QTabWidget *tabWidget);

    void addTabWithFile(ProjectFile &file);

    QTabWidget *tabWidget() {

      return m_tabWidget;
    }
    void setTabWidget(QTabWidget *tabWidget) {

      m_tabWidget = tabWidget;
    }
};

#endif // TABSHELPER_H
