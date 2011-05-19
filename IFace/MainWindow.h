#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStatusBar>
#include "MenuBar.h"
#include <QTabBar>
#include <QMenu>
#include <QSplitter>
#include <QTreeView>
#include <QTableView>
#include <QToolBar>
#include <QProcess>

#include "ProjectManager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT

	static QString m_qtAssistantEnPath;
	static QString m_qtAssistantRuPath;

    QStatusBar *m_statusBar;
	QToolBar *m_toolBar;
	QWidget *m_menuBarWrapper;
    MenuBar *m_menuBar;
	QMenu *m_lastProjectsMenu;
	QList<QShortcut*> m_shortcutList;

	QAction *m_highlightToolBarAction;
	QAction *m_highlightMenuAction;

	QList<QAction *> m_projectRelatedActions;
	QList<QAction *> m_codeRelatedActions;
	QList<QAction *> m_commentsRelatedActions;
	QList<QAction *> m_compiletRelatedActions;

	bool m_isInCompile;
	bool m_isInCode;
	bool m_isInComments;

	QProcess *m_helpProcess;
	QString m_lastEnHelpSource;
	QString m_lastRuHelpSource;

	bool m_needReopenHelp;

    ProjectManager *m_currentProjectManager;

    void initStatusBar();
    void initMenu();
    void initSplitters();
    void initProjectTree();
    void initCodeTabs();
    void initErrorTable();
	void initShortcuts();
	void initEventFilters();
	void initToolBar();

	void showStartUpHelp();

	QIcon makeIconForColor(const QColor &color);
	void updateColorIcons();

	void updateEnability();

    QMenu *createFileMenu();
	QMenu *createEditMenu();
	QMenu *createBuildMenu();
	QMenu *createLanguageMenu();
	QMenu *createHelpMenu();

	void setupProjectEnvironment(ProjectManager *projectManager);

	void updateLastProjectsMenu();

	void retranslate();
	void reopenHelp();

	void loadMenuHelp();
	void loadKeywordHelp();
	void loadCodeEditorHelp();
	void loadErrorHelp();

	void showRuSource(QString source);
	void showEnSource(QString source);

	QString trasliterate(QString rusText);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void setFocusToMenu();
	bool eventFilter(QObject *object, QEvent *event);

protected:
	void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

public slots:
	void newProjectFileSlot();
	void openProjectFileSlot();
	void removeProjectFileSlot(ProjectFile *file);

private slots:
    void newProjectSlot();
    void openProjectSlot();
	void openProjectFromLastSlot();
	void saveCurrentFileSlot();
	void saveAllSlot();
	void compile();
	void compileEnd();
	void shortcutActivated();
	void changeTextHighlightColor();
	void highlightSelectedTextSlot();
	void showStatusBarInfo(QAction *action);
	void hideStatusBarInfo();
	void paste();
	void cut();
	void copy();
	void enLang();
	void ruLang();
	void showHelp();
	void showContextHelp();
};

#endif // MAINWINDOW_H
