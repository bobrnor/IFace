#include "CodeEditorWidget.h"
#include "ui_CodeEditorWidget.h"

#include <QDebug>

#include <QVBoxLayout>

#include "CodeEditor.h"
#include "CommentsEditor.h"

CodeEditorWidget::CodeEditorWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CodeEditorWidget) {

	ui->setupUi(this);

	m_codeEditor = NULL;
	m_commentsArea = NULL;

	ui->_codeEditorWidget->setLayout(new QVBoxLayout(this));
	ui->_codeEditorWidget->layout()->setMargin(0);
	ui->_codeCommentsWidget->setLayout(new QVBoxLayout(this));
	ui->_codeCommentsWidget->layout()->setMargin(0);
}

CodeEditorWidget::~CodeEditorWidget() {

	delete ui;
	if (m_codeEditor != NULL) {
		delete m_codeEditor;
	}	
	if (m_commentsArea != NULL) {
		delete m_commentsArea;
	}	
}

void CodeEditorWidget::linkCodeWithComments() {

	connect(m_codeEditor, SIGNAL(commentsScrollRequestSignal(int)), 
		m_commentsArea, SLOT(codeScrolledSlot(int)));
	connect(m_commentsArea, SIGNAL(codeScrollRequestSignal(int)), 
		m_codeEditor, SLOT(commentsScrolledSlot(int)));
	connect(m_codeEditor, SIGNAL(commentsUpdateRequestSignal(int)), 
		m_commentsArea, SLOT(codeBlockCountChangedSlot(int)));
	connect(m_commentsArea, SIGNAL(applyCommentsSignal()), 
		m_codeEditor, SLOT(applyCommentsSlot()), Qt::DirectConnection);
	connect(m_codeEditor, SIGNAL(codeCursorLineChangedSignal(int)), 
		m_commentsArea, SLOT(codePositionChangedSlot(int)), Qt::DirectConnection);
	connect(m_commentsArea, SIGNAL(commentsCursorLineChangedSignal(int)), 
		m_codeEditor, SLOT(commentsPositionChangedSlot(int)), Qt::DirectConnection);
	connect(m_commentsArea, SIGNAL(commentsChangedSignal()), m_codeEditor, SLOT(textChangedSlot()));

	m_commentsArea->codeBlockCountChangedSlot(m_codeEditor->blockCount());
}

CodeEditor *CodeEditorWidget::codeEditor() const {

	return m_codeEditor;
}
void CodeEditorWidget::setCodeEditor(CodeEditor *codeEditor) {

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

	return m_commentsArea;
}
void CodeEditorWidget::setCommentsArea(CommentsEditor *commentsArea) {

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