#ifndef TABSHELPER_H
#define TABSHELPER_H

#include <QObject>
#include <QTabWidget>
#include <QList>

#include "ProjectFile.h"

class TabsHelper : public QObject {
Q_OBJECT

  QTabWidget *m_tabWidget;
  QList<ProjectFile *> m_openFiles;

  bool isFileAlreadyOpen(ProjectFile *file) const;

public:
    TabsHelper();
    TabsHelper(QTabWidget *tabWidget);

    void showTabWithFile(ProjectFile *file);

    QTabWidget *tabWidget() { return m_tabWidget; }
    void setTabWidget(QTabWidget *tabWidget);

	ProjectFile *currentlyOpenProjectFile();
	QMap<uint, ProjectFile *> openFiles();

	bool tryCloseAll();
	bool tryCloseTab(int index);

public slots:
	void tabCloseRequestSlot(int index);
	void modificationChangedSlot(bool changed);

signals:
	void codeCursorChangedSignal(ProjectFile *file, int xPos, int yPos);
	void codeChangedSignal(ProjectFile *file, int xPos, int yPos);
};

#endif // TABSHELPER_H
