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
    QMenuBar *m_menuBar;

    ProjectManager *m_currentProjectManager;

    void initStatusBar();
    void initMenu();
    void initSplitters();
    void initProjectTree();
    void initCodeTabs();
    void initErrorTable();
    QMenu *createFileMenu();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void newProjectSlot();
};

#endif // MAINWINDOW_H
