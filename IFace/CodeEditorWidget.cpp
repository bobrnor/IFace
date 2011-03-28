#include "CodeEditorWidget.h"
#include "ui_CodeEditorWidget.h"

#include <QDebug>

#include <QVBoxLayout>

#include "CodeEditor.h"
#include "CommentsEditor.h"

CodeEditorWidget::CodeEditorWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CodeEditorWidget) {

	qDebug() << __FUNCSIG__;
	ui->setupUi(this);

	m_codeEditor = NULL;
	m_commentsArea = NULL;

	ui->_codeEditorWidget->setLayout(new QVBoxLayout(this));
	ui->_codeEditorWidget->layout()->setMargin(0);
	ui->_codeCommentsWidget->setLayout(new QVBoxLayout(this));
	ui->_codeCommentsWidget->layout()->setMargin(0);
}

CodeEditorWidget::~CodeEditorWidget() {

	qDebug() << __FUNCSIG__;
	delete ui;
	if (m_codeEditor != NULL) {
		delete m_codeEditor;
	}	
	if (m_commentsArea != NULL) {
		delete m_commentsArea;
	}	
}

void CodeEditorWidget::linkCodeWithComments() {

	qDebug() << __FUNCSIG__;
	connect(m_codeEditor, SIGNAL(commentsScrollRequestSignal(int)), 
		m_commentsArea, SLOT(codeScrolledSlot(int)));
	connect(m_codeEditor, SIGNAL(commentsUpdateRequestSignal(int)), 
		m_commentsArea, SLOT(codeBlockCountChangedSlot(int)));
}

CodeEditor *CodeEditorWidget::codeEditor() const {

	qDebug() << __FUNCSIG__;
	return m_codeEditor;
}
void CodeEditorWidget::setCodeEditor(CodeEditor *codeEditor) {

	qDebug() << __FUNCSIG__;
	Q_ASSERT(codeEditor);
	if (m_codeEditor != NULL) {
		disconnect(m_codeEditor);
		ui->_codeEditorWidget->layout()->removeWidget(m_codeEditor);
		delete m_codeEditor;
	}
	m_codeEditor = codeEditor;
	ui->_codeEditorWidget->layout()->addWidget(m_codeEditor);

	if (m_codeEditor != NULL && m_commentsArea != NULL) {
		linkCodeWithComments();
	}
}
CommentsEditor *CodeEditorWidget::commentsArea() const {

	qDebug() << __FUNCSIG__;
	return m_commentsArea;
}
void CodeEditorWidget::setCommentsArea(CommentsEditor *commentsArea) {

	qDebug() << __FUNCSIG__;
	Q_ASSERT(commentsArea);
	if (m_commentsArea != NULL) {
		disconnect(m_commentsArea);
		ui->_codeCommentsWidget->layout()->removeWidget(m_commentsArea);
		delete m_commentsArea;
	}
	m_commentsArea = commentsArea;
	ui->_codeCommentsWidget->layout()->addWidget(m_commentsArea);

	if (m_codeEditor != NULL && m_commentsArea != NULL) {
		linkCodeWithComments();
	}
}