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
#include <QDir>
#include <QFont>
#include <QMenu>

#include "LeftArea.h"
#include "CodeLineData.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {

	qDebug() << __FUNCSIG__;
	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	setFont(font);

	setUndoRedoEnabled(false);

	QShortcut *shortcut = new QShortcut(this);
	shortcut->setKey(QKeySequence("F8"));
	shortcut->setEnabled(false);
	connect(shortcut, SIGNAL(activated()), this, SLOT(changeBreakPointSlot()));
	m_shortcutList.append(shortcut);

	m_isLastUpdateRequestFromComments = false;
	m_isLastUpdateRequestFromErrorTable = false;
	m_lastCommentOffsetLine = -1;
	m_isInit = false;
	m_isChanged = false;
	m_isLoaded = false;

	int m_currentXErrorPosition = -1;
	int m_currentYErrorPosition = -1;

	m_projectFile = NULL;

	setWordWrapMode(QTextOption::NoWrap);
	setLayout(new QVBoxLayout(this));

	m_leftArea = new LeftArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(blockCountChangedSlot(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateRequestSlot(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLineSlot()));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledSlot(int)));
	connect(this, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));

	blockCountChangedSlot(0);
	highlightCurrentLineSlot();

	connect(this->document(), SIGNAL(contentsChange(int, int, int)), 
		this, SLOT(contentsChangedSlot(int, int, int)));
	connect(m_leftArea, SIGNAL(doubleClickSignal(int, int)), 
		this, SLOT(leftAreaDoubleClickSlot(int, int)));
}

CodeEditor::~CodeEditor() {

	qDebug() << __FUNCSIG__;
	if (m_projectFile != NULL) {
		m_projectFile->linkCodeEditor(NULL);
	}
}

int CodeEditor::leftAreaWidth() {

	qDebug() << __FUNCSIG__;
	int max = qMax(1, blockCount());
	int digits = 5 + log10((double) max);

	return 25 + fontMetrics().width(QLatin1Char('9')) * digits;
}

void CodeEditor::blockCountChangedSlot(int newBlockCount) {

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
	if (dy) {
		m_leftArea->scroll(0, dy);
	}
	else {
		m_leftArea->update(0, rect.y(), leftAreaWidth(), rect.height());
	}

// 	if (rect.contains(viewport()->rect())) {
// 		blockCountChangedSlot(0);
// 	}
}

void CodeEditor::scrolledSlot(int y) {

	qDebug() << __FUNCSIG__;
	if (!m_isLastUpdateRequestFromComments) {
		emit commentsScrollRequestSignal(y);
	}

	m_isLastUpdateRequestFromComments = false;
}

void CodeEditor::updateBreakPointAndComments() {

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_leftArea->setGeometry(cr.left(), cr.top(), leftAreaWidth(), cr.height());
}

void CodeEditor::highlightCurrentLineSlot() {

	qDebug() << __FUNCSIG__;
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly() && hasFocus()) {
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);

		if (!m_isLastUpdateRequestFromErrorTable) {
			emit codeCursorChangedSignal(m_projectFile, textCursor().positionInBlock(), textCursor().blockNumber());
		}
	}

	updateErrors();

	extraSelections.append(m_errorSelections);
	extraSelections.append(m_errorSymbols);

	setExtraSelections(extraSelections);
	
	if (!m_isLastUpdateRequestFromComments) {
		emit codeCursorLineChangedSignal(textCursor().blockNumber());
	}

	m_isLastUpdateRequestFromComments = false;
	m_isLastUpdateRequestFromErrorTable = false;
}

void CodeEditor::leftAreaPaintEvent(QPaintEvent *event) {

	qDebug() << __FUNCSIG__;
	QPainter painter(m_leftArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			int count = 0;
			foreach (CompileError error, m_projectFile->compileErrorsEn()) {
				if (error.yPos() == blockNumber + 1) {
					count++;
				}
			}

			QString number = QString::number(blockNumber + 1)
				.append(" (")
				.append(QString::number(count)).append(")");

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

ProjectFile *CodeEditor::projectFile() const {

	qDebug() << __FUNCSIG__;
	return m_projectFile;
}

void CodeEditor::setProjectFile(ProjectFile *projectFile) {

	qDebug() << __FUNCSIG__;
	if (m_projectFile != NULL) {
		m_projectFile->linkCodeEditor(NULL);
	}
	m_projectFile = projectFile;
	m_projectFile->linkCodeEditor(this);
	loadProjectFile();
}

void CodeEditor::loadProjectFile() {

	qDebug() << __FUNCSIG__;
	if (m_projectFile != NULL) {
		QFile file(m_projectFile->path());
		if (file.open(QIODevice::ReadOnly)) {

			QTextStream stream(&file);
			while (!stream.atEnd()) {
				QString codeline = stream.readAll();
				insertPlainText(codeline);
			}
		}
		initCommentsAndBreakPoints();
		m_isLoaded = true;
	}
}

void CodeEditor::initCommentsAndBreakPoints() {

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
	if (m_projectFile != NULL) {
		m_projectFile->setTmpPath("");
		QString path = m_projectFile->path();
		QFile file(path);
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream textStream(&file);
			textStream << toPlainText();
		}
		//m_isChanged = false;
		emit modificationChanged(false);
	}
}

void CodeEditor::tempSaveProjectFile() {

	qDebug() << __FUNCSIG__;
	if (m_projectFile != NULL) {
		QTemporaryFile tempFile;
		tempFile.setAutoRemove(false);

		if(tempFile.open()) {
			QTextStream textStream(&tempFile);
			textStream << toPlainText();
		}

		QString tmpPath = tempFile.fileName();
		m_projectFile->setTmpPath(tmpPath);

		tempFile.close();
	}
}

void CodeEditor::updateErrors() {

	qDebug() << __FUNCSIG__;
	if (m_projectFile != NULL) {

		m_errorSymbols.clear();
		foreach (CompileError error, m_projectFile->compileErrorsEn()) {
			if (!error.isValid()) {
				continue;
			}

			int line = error.yPos() - 1;
			int character = error.xPos() - 1;

			QTextBlock textBlock = this->document()->findBlockByLineNumber(line);
			QTextCursor cursor(textBlock);
			
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, character);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);

			QTextEdit::ExtraSelection selection;
			QColor lineColor;
			if (line == m_currentYErrorPosition && character == m_currentXErrorPosition) {
				lineColor = QColor(Qt::red).lighter(80);
			}
			else {
				lineColor = QColor(Qt::red).lighter(140);
			}

			selection.format.setBackground(lineColor);
			selection.cursor = cursor;
			m_errorSymbols.append(selection);
		}
	}
}

void CodeEditor::update() {

	qDebug() << __FUNCSIG__;
	highlightCurrentLineSlot();
}

void CodeEditor::focusInEvent(QFocusEvent *e) {

	qDebug() << __FUNCSIG__;
	foreach (QShortcut *shortcut, m_shortcutList) {
		shortcut->setEnabled(true);
	}
	//updateErrors();
	m_currentXErrorPosition = -1;
	m_currentYErrorPosition = -1;
	highlightCurrentLineSlot();
}

void CodeEditor::focusOutEvent(QFocusEvent *e) {

	qDebug() << __FUNCSIG__;
	foreach (QShortcut *shortcut, m_shortcutList) {
		shortcut->setEnabled(false);
	}
	highlightCurrentLineSlot();
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {

	qDebug() << __FUNCSIG__;
	QTextCursor cursor = textCursor();
	int blockNumber = cursor.blockNumber();
	int positionInBlock = cursor.positionInBlock();
	int key = e->key();

	if ((key == Qt::Key_Enter || key == Qt::Key_Return) && positionInBlock ==  0) {
		m_lastCommentOffsetLine = blockNumber;
	}

	QPlainTextEdit::keyPressEvent(e);
}

void CodeEditor::contentsChangedSlot(int position, int charsRemoved, int charsAdded) {

	qDebug() << __FUNCSIG__;
	QTextCursor cursor(textCursor());
	cursor.setPosition(position);
	qDebug() << "Changed from " << cursor.blockNumber() << ", " << cursor.positionInBlock();
	emit codeChangedSignal(m_projectFile, cursor.positionInBlock(), cursor.blockNumber());
}

void CodeEditor::moveDownComments(int fromBlockNumber) {

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
	return this->document()->findBlockByNumber(blockNumber);
}

void CodeEditor::applyCommentsSlot() {

	qDebug() << __FUNCSIG__;
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

	qDebug() << __FUNCSIG__;
	verticalScrollBar()->setValue(y);
}

void CodeEditor::commentsPositionChangedSlot(int yPos) {

	qDebug() << __FUNCSIG__;
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
	m_isLastUpdateRequestFromComments = true;
	setTextCursor(cursor);
}

void CodeEditor::errorPositionChangedSlot(int xPos, int yPos) {

	qDebug() << __FUNCSIG__;
	m_isLastUpdateRequestFromErrorTable = true;
	m_currentXErrorPosition = xPos - 1;
	m_currentYErrorPosition = yPos - 1;

	QTextCursor cursor = textCursor();
	int delta = cursor.blockNumber() - yPos + 1;
	if (delta != 0) {
		if (delta > 0) {
			cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, qAbs(delta));
		}
		else {
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, qAbs(delta));
		}
	}

	m_errorSelections.clear();

	// highlight for line
	QTextEdit::ExtraSelection selection;
	QColor lineColor = QColor(Qt::red).lighter(180);

	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = QTextCursor(cursor);
	selection.cursor.clearSelection();
	m_errorSelections.append(selection);

	delta = cursor.positionInBlock() - xPos + 1;
	if (delta != 0) {
		if (delta > 0) {
			cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, qAbs(delta));
		}
		else {
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, qAbs(delta));
		}
	}

	setTextCursor(cursor);
}

void CodeEditor::textChangedSlot() {

	qDebug() << __FUNCSIG__;
	if (m_isLoaded) {
		//m_isChanged = true;
		emit modificationChanged(true);
	}
}

void CodeEditor::changeBreakPointSlot() {

	qDebug() << __FUNCSIG__;
	QTextBlock block = textCursor().block();
	CodeLineData *data = NULL;
	if (block.userData() != NULL) {
		data = static_cast<CodeLineData *>(block.userData());
	}
	else {
		data = new CodeLineData();
	}
	data->hasBreakPoint = !data->hasBreakPoint;
	block.setUserData(data);
	updateBreakPointAndComments();

	//m_isChanged = true;
	emit modificationChanged(true);

	m_leftArea->updateGeometry();
}

void CodeEditor::leftAreaDoubleClickSlot(int x, int y) {

	qDebug() << __FUNCSIG__;
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && block.isVisible()) {
		qDebug() << top << " <= " << y << " < " << bottom;
		if (y >= top && y < bottom) {
			CodeLineData *data = NULL;
			if (block.userData() != NULL) {
				data = static_cast<CodeLineData *>(block.userData());
			}
			else {
				data = new CodeLineData();
			}
			data->hasBreakPoint = !data->hasBreakPoint;
			block.setUserData(data);
			updateBreakPointAndComments();

			emit modificationChanged(true);

			m_leftArea->updateGeometry();
			break;
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
	}
}

void CodeEditor::setCheckStatusForAll(bool checked) {

	qDebug() << __FUNCSIG__;
	QTextBlock block = blockWithNumber(0);

	while (block.isValid()) {
		CodeLineData *data = NULL;
		if (block.userData() != NULL) {
			data = static_cast<CodeLineData *>(block.userData());
		}
		else {
			data = new CodeLineData();
		}
		data->hasBreakPoint = checked;
		block.setUserData(data);

		block = block.next();
	}

	updateBreakPointAndComments();
	emit modificationChanged(true);
	m_leftArea->updateGeometry();
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event) {

	QMenu *menu = createStandardContextMenu();
	menu->exec(event->globalPos());
}