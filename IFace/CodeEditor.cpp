#include "CodeEditor.h"

#include <cmath>
#include <QDebug>
#include <QTextEdit>
#include <QList>
#include <QFile>
#include <QColor>
#include <QPainter>
#include <QTextBlock>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QScrollBar>
#include <QInputMethodEvent>

#include "LeftArea.h"
#include "CodeLineData.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {

	m_isLastUpdateRequestFromComments = false;
	m_lastCommentOffsetLine = -1;
	m_isInit = false;

	setWordWrapMode(QTextOption::NoWrap);
	setLayout(new QVBoxLayout(this));

	m_leftArea = new LeftArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(blockCountChangedSlot(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateRequestSlot(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLineSlot()));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledSlot(int)));

	blockCountChangedSlot(0);
	highlightCurrentLineSlot();
}

CodeEditor::~CodeEditor() {

	if (m_projectFile != NULL) {
		m_projectFile->linkCodeEditor(NULL);
	}
}

int CodeEditor::leftAreaWidth() {

	int max = qMax(1, blockCount());
	int digits = 1 + log10((double) max);
	qDebug() << "Digit count: " << digits;

	return 25 + fontMetrics().width(QLatin1Char('9')) * digits;
}

void CodeEditor::blockCountChangedSlot(int newBlockCount) {

	setViewportMargins(leftAreaWidth(), 0, 0, 0);

	if (!m_isLastUpdateRequestFromComments) {
		updateBreakPointAndComments();

		emit commentsUpdateRequestSignal(newBlockCount);

		int scroll = verticalScrollBar()->value();
		emit commentsScrollRequestSignal(scroll);
	}

	m_isLastUpdateRequestFromComments = false;
}

void CodeEditor::updateRequestSlot(const QRect &rect, int dy) {

	if (dy) {
		m_leftArea->scroll(0, dy);
	}
	else {
		m_leftArea->update(0, rect.y(), leftAreaWidth(), rect.height());
	}

	if (rect.contains(viewport()->rect())) {
		//blockCountChangedSlot(0);
	}
}

void CodeEditor::scrolledSlot(int y) {

	if (!m_isLastUpdateRequestFromComments) {
		emit commentsScrollRequestSignal(y);
	}

	m_isLastUpdateRequestFromComments = false;
}

void CodeEditor::updateBreakPointAndComments() {

	if (m_isInit) {
		if (m_lastCommentOffsetLine >= 0) {
			moveDownComments(m_lastCommentOffsetLine);
			m_lastCommentOffsetLine = -1;
		}	

		if (m_projectFile != NULL) {
			QTextBlock block = firstVisibleBlock();

			while (block.isValid() && block.isVisible()) {
				CodeLineData *data = static_cast<CodeLineData *>(block.userData());
				if (data == NULL) {
					m_projectFile->removeBreakPointIfExists(block.blockNumber());
					m_projectFile->removeCommentIfExists(block.blockNumber());
				}
				else {
					if (data->comment.isEmpty()) {
						m_projectFile->removeCommentIfExists(block.blockNumber());
					}
					else {
						m_projectFile->addOrReplaceComment(block.blockNumber(), data->comment);
					}

					if (data->hasBreakPoint) {
						m_projectFile->addBreakPoint(block.blockNumber());
					}
					else {
						m_projectFile->removeBreakPointIfExists(block.blockNumber());
					}
				}

				block = block.next();
			}
		}
	}
}

void CodeEditor::resizeEvent(QResizeEvent *e) {

	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_leftArea->setGeometry(cr.left(), cr.top(), leftAreaWidth(), cr.height());
}

void CodeEditor::highlightCurrentLineSlot() {

	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly() && hasFocus()) {
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
	
	emit codeCursorLineChangedSignal(textCursor().blockNumber());
}

void CodeEditor::leftAreaPaintEvent(QPaintEvent *event) {

	QPainter painter(m_leftArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, m_leftArea->width(), fontMetrics().height(),
				Qt::AlignRight, number);

			bool isWithBreakPoint = m_projectFile->checkLineForBreakPoints(blockNumber);
			if (isWithBreakPoint) {
				painter.drawRect(6, top + 3, 10, 6);
				painter.fillRect(6, top + 3, 10, 6, Qt::green);
			}
			else {
				painter.drawRect(6, top + 3, 10, 6);
			}		
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

SProjectFile CodeEditor::projectFile() const {

	return m_projectFile;
}

void CodeEditor::setProjectFile(SProjectFile projectFile) {

	if (m_projectFile != NULL) {
		m_projectFile->linkCodeEditor(NULL);
	}
	m_projectFile = projectFile;
	m_projectFile->linkCodeEditor(this);
	loadProjectFile();
}

void CodeEditor::loadProjectFile() {

	if (m_projectFile != NULL) {

		QFile file(m_projectFile->path());
		if (file.open(QIODevice::ReadOnly)) {

			QTextStream stream(&file);
			while (!stream.atEnd()) {
				QString codeline = stream.readLine() + "\n";
				insertPlainText(codeline);
			}
		}
		initCommentsAndBreakPoints();
	}
}

void CodeEditor::initCommentsAndBreakPoints() {

	QTextBlock block = blockWithNumber(0);
	
	while (block.isValid()) {
		CodeLineData *data = new CodeLineData();

		if (m_projectFile->checkLineForBreakPoints(block.blockNumber())) {			
			data->hasBreakPoint = true;
		}

		QString comment = m_projectFile->commentInLine(block.blockNumber());
		if (!comment.isEmpty()) {
			data->comment = comment;
		}

		block.setUserData(data);
		block = block.next();
	}
	m_isInit = true;
}

void CodeEditor::saveProjectFile() {

	if (!m_projectFile.isNull()) {
		QString path = m_projectFile->path();
		QFile file(path);
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream textStream(&file);
			textStream << toPlainText();
		}
	}
}

void CodeEditor::focusInEvent(QFocusEvent *e) {

	highlightCurrentLineSlot();
}

void CodeEditor::focusOutEvent(QFocusEvent *e) {

	highlightCurrentLineSlot();
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {

	QTextCursor cursor = textCursor();
	int blockNumber = cursor.blockNumber();
	int positionInBlock = cursor.positionInBlock();
	int key = e->key();

	if ((key == Qt::Key_Enter || key == Qt::Key_Return) && positionInBlock ==  0) {
		m_lastCommentOffsetLine = blockNumber;
	}
	
	QPlainTextEdit::keyPressEvent(e);
}

void CodeEditor::moveDownComments(int fromBlockNumber) {

	QTextBlock block = blockWithNumber(fromBlockNumber);
	
	if (block.userData() != NULL) {
		CodeLineData *nextLineData = static_cast<CodeLineData *>(block.userData())->copy();

		block.setUserData(NULL);

		QTextBlock nextBlock = block.next();
		if (nextBlock.isValid()) {
			nextBlock.setUserData(nextLineData);
		}
	}
}

QTextBlock CodeEditor::blockWithNumber(int blockNumber) {

// 	QTextCursor cursor = textCursor();
// 
// 	int delta = cursor.blockNumber() - blockNumber;
// 	if (delta > 0) {
// 		cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, delta);
// 	}
// 	else if (delta < 0) {
// 		cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, -delta);
// 	}
// 	return cursor.block();
	return this->document()->findBlockByNumber(blockNumber);
}

void CodeEditor::applyCommentsSlot() {

	QMap<int, QString> comments = m_projectFile->comments();
	foreach (int key, comments.keys()) {
		QString comment = comments.take(key);
		QTextBlock block = blockWithNumber(key);
		if (block.isValid()) {
			CodeLineData *data = static_cast<CodeLineData *>(block.userData());
			if (data == NULL) {
				data = new CodeLineData();
			}
			data->comment = comment;
			block.setUserData(data);
		}
	}
}

void CodeEditor::commentsScrolledSlot(int y) {

	verticalScrollBar()->setValue(y);
}

void CodeEditor::commentsPositionChangedSlot(int yPos) {

	QTextCursor cursor = textCursor();
	int delta = cursor.blockNumber() - yPos;
	if (delta != 0) {
		if (delta > 0) {
			cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, qAbs(delta));
		}
		else {
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, qAbs(delta));
		}		
	}
	setTextCursor(cursor);
}
