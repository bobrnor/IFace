#ifndef TABSHELPER_H
#define TABSHELPER_H

#include <QTabWidget>
#include <QList>

#include "ProjectFile.h"

class ProjectFile;

class TabsHelper {

  QTabWidget *m_tabWidget;
  QList<SProjectFile> m_openFiles;

  bool isFileAlreadyOpen(ProjectFile *file) const;

public:
    TabsHelper();
    TabsHelper(QTabWidget *tabWidget);

    void addTabWithFile(SProjectFile file);

    QTabWidget *tabWidget() { return m_tabWidget; }
    void setTabWidget(QTabWidget *tabWidget) { m_tabWidget = tabWidget; }

	SProjectFile currentlyOpenProjectFile();
};

#endif // TABSHELPER_H
