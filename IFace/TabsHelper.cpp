#include "TabsHelper.h"

#include <QDebug>
#include <QMessageBox>

#include "CodeEditor.h"
#include "CommentsEditor.h"
#include "CodeEditorWidget.h"
#include "Project.h"

TabsHelper::TabsHelper() {

	m_tabWidget = NULL;
}

TabsHelper::TabsHelper(QTabWidget *tabWidget) {

	qDebug() << __FUNCSIG__;
	m_tabWidget = NULL;
	setTabWidget(tabWidget);	
}

void TabsHelper::setTabWidget(QTabWidget *tabWidget) { 

	if (m_tabWidget != NULL) {
		disconnect(m_tabWidget);
		delete m_tabWidget;
	}
	m_tabWidget = tabWidget; 
	int rand = qrand();
	qDebug() << "Id: " << rand;
	m_tabWidget->setStatusTip(QString::number(rand));
	connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), 
		this, SLOT(tabCloseRequestSlot(int)));
}

void TabsHelper::showTabWithFile(ProjectFile *file) {

	Q_ASSERT(m_tabWidget != NULL);

	int tabNumber = -1;

	if (!isFileAlreadyOpen(file)) {
		CodeEditorWidget *widget = new CodeEditorWidget(m_tabWidget);

		CodeEditor *codeEditor = new CodeEditor(widget);
		codeEditor->setProjectFile(file);		

		connect(codeEditor, SIGNAL(codeCursorChangedSignal(ProjectFile*, int, int)), 
			this, SIGNAL(codeCursorChangedSignal(ProjectFile*, int, int)));
		connect(codeEditor, SIGNAL(codeChangedSignal(ProjectFile*, int, int)), 
			this, SIGNAL(codeChangedSignal(ProjectFile*, int, int)));
		connect(codeEditor, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChangedSlot(bool)));

		CommentsEditor *commentsArea = new CommentsEditor(widget);		
		commentsArea->setProjectFile(file);

		widget->setCodeEditor(codeEditor);
		widget->setCommentsArea(commentsArea);		

		tabNumber = m_tabWidget->addTab(widget, file->fileName());
		m_openFiles.append(file);
	}
	else {
		if (m_tabWidget != NULL) {
			int widgetCount = m_tabWidget->count();
			for (int i = 0; i < widgetCount; ++i) {
				QWidget *widget = m_tabWidget->widget(i);
				if (widget != NULL) {
					CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(widget);
					CodeEditor *codeEditor = codeEditorWidget->codeEditor();
					if (codeEditor->projectFile()->path() == file->path()) {
						tabNumber = i;
						break;
					}
				}
			}
		}
	}
	if (tabNumber >= 0) {
		m_tabWidget->setCurrentIndex(tabNumber);
	}
}

bool TabsHelper::isFileAlreadyOpen(ProjectFile *file) const {

	bool result = false;
	foreach (ProjectFile *sfile, m_openFiles) {
		if (sfile->path() == file->path()) {
			result = true;
			break;
		}
	}
	return result;
}

ProjectFile *TabsHelper::currentlyOpenProjectFile() {

	if (m_tabWidget != NULL) {
		QWidget *curentWidget = m_tabWidget->currentWidget();
		if (curentWidget != NULL) {
			CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(curentWidget);
			ProjectFile *projectFile = codeEditorWidget->codeEditor()->projectFile();
			return projectFile;
		}
	}
	return NULL;
}

QMap<uint, ProjectFile *> TabsHelper::openFiles() {

	QMap<uint, ProjectFile *> result;

	if (m_tabWidget != NULL) {
		int widgetCount = m_tabWidget->count();
		for (int i = 0; i < widgetCount; ++i) {
			QWidget *widget = m_tabWidget->widget(i);
			if (widget != NULL) {
				CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(widget);
				CodeEditor *codeEditor = codeEditorWidget->codeEditor();
				codeEditor->tempSaveProjectFile();
				ProjectFile *projectFile = codeEditor->projectFile();
				result[projectFile->hash()] = projectFile;
			}
		}
	}

	return result;
}

bool TabsHelper::tryCloseAll() {

	bool result = true;
	for (int i = m_tabWidget->count() - 1; i >= 0 && result; --i) {
		result = tryCloseTab(i);
	}
	return result;
}

bool TabsHelper::tryCloseTab(int index) {

	qDebug() << "rem_id: " << m_tabWidget->statusTip();
	CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(m_tabWidget->widget(index));
	CodeEditor *codeEditor = codeEditorWidget->codeEditor();

	if (codeEditor->isChanged()) {
		m_tabWidget->setCurrentIndex(index);
		QMessageBox saveRequestBox;
		saveRequestBox.setWindowTitle(codeEditor->projectFile()->fileName());
		saveRequestBox.setText(tr("File has been modified."));
		saveRequestBox.setInformativeText(tr("Do you want to save your changes?"));
		saveRequestBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		saveRequestBox.setDefaultButton(QMessageBox::Save);
		int result = saveRequestBox.exec();

		switch (result) {
		case QMessageBox::Save:			
			codeEditor->projectFile()->project()->saveProjectFile(codeEditor->projectFile());

		case QMessageBox::Discard:
			m_openFiles.removeAll(codeEditor->projectFile());
			m_tabWidget->removeTab(index);
			return true;

		case QMessageBox::Cancel:
			return false;

		default:
			Q_ASSERT(false);
			return false;
		}
	}
	else {
		m_openFiles.removeAll(codeEditor->projectFile());
		m_tabWidget->removeTab(index);
		return true;
	}
}

void TabsHelper::tabCloseRequestSlot(int index) {

	QWidget *widget = m_tabWidget->widget(index);
	int tryCloseResult = tryCloseTab(index);
	if (tryCloseResult) {
		delete widget;
	}
}

void TabsHelper::modificationChangedSlot(bool changed) {

	if (sender() != NULL) {
		CodeEditor *codeEditor = static_cast<CodeEditor *>(sender());
		QString senderPath = codeEditor->projectFile()->path();

		for (int i = 0; i < m_tabWidget->count(); ++i) {
			CodeEditorWidget *codeEditorWidget = static_cast<CodeEditorWidget *>(m_tabWidget->widget(i));
			CodeEditor *codeEditor = codeEditorWidget->codeEditor();
			QString path = codeEditor->projectFile()->path();
			if (senderPath == path) {
				QString name = codeEditor->projectFile()->fileName();
				codeEditor->setIsChanged(changed);
				QString tabText = name + ((changed) ? "*" : "");
				m_tabWidget->setTabText(i, tabText);
				break;
			}
		}
	}
}