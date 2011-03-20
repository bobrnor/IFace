#include "CodeEditorWidget.h"
#include "ui_CodeEditorWidget.h"

#include <QVBoxLayout>

#include "CodeEditor.h"
#include "CommentsArea.h"

CodeEditorWidget::CodeEditorWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CodeEditorWidget) {

	ui->setupUi(this);

	m_codeEditor = NULL;
	m_commentsArea = NULL;

	ui->_codeEditorWidget->setLayout(new QVBoxLayout(this));
	ui->_codeEditorWidget->layout()->setMargin(0);
	ui->_codeCommentsWidget->setLayout(new QVBoxLayout(this));
	ui->_codeCommentsWidget->layout()->setMargin(0);
}

CodeEditor *CodeEditorWidget::codeEditor() const {

	return m_codeEditor;
}
void CodeEditorWidget::setCodeEditor(CodeEditor *codeEditor) {

	Q_ASSERT(codeEditor);
	if (m_codeEditor != NULL) {
		ui->_codeEditorWidget->layout()->removeWidget(m_codeEditor);
		delete m_codeEditor;
	}
	m_codeEditor = codeEditor;
	ui->_codeEditorWidget->layout()->addWidget(m_codeEditor);
}
CommentsArea *CodeEditorWidget::commentsArea() const {

	return m_commentsArea;
}
void CodeEditorWidget::setCommentsArea(CommentsArea *commentsArea) {

	Q_ASSERT(commentsArea);
	if (m_commentsArea != NULL) {
		ui->_codeCommentsWidget->layout()->removeWidget(m_commentsArea);
		delete m_commentsArea;
	}
	m_commentsArea = commentsArea;
	ui->_codeCommentsWidget->layout()->addWidget(m_commentsArea);
}