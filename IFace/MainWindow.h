#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QTabBar>
#include <QMenu>
#include <QSplitter>
#include <QTreeView>
#include <QTableView>

#include "ProjectManager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT

    QStatusBar *m_statusBar;
	QWidget *m_menuBarWrapper;
    QMenuBar *m_menuBar;
	QMenu *m_lastProjectsMenu;
	QList<QShortcut*> m_shortcutList;

    ProjectManager *m_currentProjectManager;

    void initStatusBar();
    void initMenu();
    void initSplitters();
    void initProjectTree();
    void initCodeTabs();
    void initErrorTable();
	void initShortcuts();
	void initEventFilters();

    QMenu *createFileMenu();

	void setupProjectEnvironment(ProjectManager *projectManager);

	void updateLastProjectsMenu();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void setFocusToMenu();
	bool eventFilter(QObject *object, QEvent *event);

protected:
	void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

private slots:
    void newProjectSlot();
    void newProjectFileSlot();
    void openProjectSlot();
	void openProjectFromLastSlot();
    void openProjectFileSlot();
	void saveCurrentFileSlot();
	void saveAllSlot();
	void compile();
	void shortcutActivated();
};

#endif // MAINWINDOW_H
