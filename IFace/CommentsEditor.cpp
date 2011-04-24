#include "CommentsEditor.h"

#include <QDebug>

#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include <QFile>

CommentsEditor::CommentsEditor(QWidget *parent /* = 0 */) : QPlainTextEdit(parent) {

	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	setFont(font);

	m_codeBlockCount = 0;
	m_changingBlockCount = false;
	m_isLastUpdateRequestFromCode = false;
	m_isInit = false;
	m_skipModifications = false;
	setWordWrapMode(QTextOption::NoWrap);
	//setMaximumBlockCount(0);

	m_projectFile = NULL;

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLineSlot()));
	connect(this, SIGNAL(textChanged()), this, SLOT(commentsChangedSlot()), Qt::DirectConnection);
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(blockCountChangedSlot(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledSlot(int)));

	highlightCurrentLineSlot();
}

CommentsEditor::~CommentsEditor() {

}

void CommentsEditor::setProjectFile(ProjectFile *projectFile) {

	m_projectFile = projectFile;
	if (!m_projectFile != NULL) {
		QFile file(m_projectFile->path());
		if (file.open(QIODevice::ReadOnly)) {
			QTextStream stream(&file);
			while (!stream.atEnd()) {
				stream.readLine();
				insertPlainText("\n");
			}
		}
	}
	updateComments();
	m_isInit = true;
}

void CommentsEditor::highlightCurrentLineSlot() {

	m_skipModifications = true;
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

	if (!m_isLastUpdateRequestFromCode) {
		emit commentsCursorLineChangedSignal(textCursor().blockNumber());
	}
	m_isLastUpdateRequestFromCode = false;
	m_skipModifications = false;
}

void CommentsEditor::focusInEvent(QFocusEvent *e) {

	highlightCurrentLineSlot();
}

void CommentsEditor::focusOutEvent(QFocusEvent *e) {

	saveComments();
	highlightCurrentLineSlot();
}

void CommentsEditor::codeScrolledSlot(int y) {

	m_isLastUpdateRequestFromCode = true;
	verticalScrollBar()->setValue(y);
}

void CommentsEditor::codeBlockCountChangedSlot(int newBlockCount) {

	if (newBlockCount > 0) {
		m_codeBlockCount = newBlockCount;
		makeProperLineCount(newBlockCount);
	}
	updateComments();
}

void CommentsEditor::blockCountChangedSlot(int newBlockCount) {

	if (m_isInit && !m_changingBlockCount && newBlockCount != m_codeBlockCount) {
		makeProperLineCount(m_codeBlockCount);
		saveComments();
	}
	int scroll = verticalScrollBar()->value();
	emit codeScrollRequestSignal(scroll);
}

void CommentsEditor::makeProperLineCount(int lineCount) {

	//setMaximumBlockCount(lineCount);
	m_skipModifications = true;

	if (m_isInit) {
		QTextCursor cursor(firstVisibleBlock());

		int lastBlockNumber = gotoEnd(cursor);

		if (lastBlockNumber != lineCount - 1) {
			m_changingBlockCount = true;
			if (lastBlockNumber > lineCount - 1) {
				int delta = lastBlockNumber - lineCount + 1;
				cursor.movePosition(QTextCursor::EndOfBlock);
				cursor.movePosition(QTextCursor.PreviousBlock, QTextCursor::KeepAnchor, delta);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
			}
			else {
				int delta = lineCount - lastBlockNumber - 1;
				for (int i = 0; i < delta; ++i) {
					cursor.insertBlock();
				}			
			}
			m_changingBlockCount = false;
		}
	}
	m_skipModifications = false;
}

void CommentsEditor::gotoBegin(QTextCursor &cursor) {

	m_skipModifications = true;
	while (cursor.movePosition(QTextCursor::PreviousBlock)) {}
	m_skipModifications = false;
}

int CommentsEditor::gotoEnd(QTextCursor &cursor) {

	m_skipModifications = true;
	int lastBlockNumber = cursor.blockNumber();
	while (cursor.movePosition(QTextCursor::NextBlock)) {
		lastBlockNumber = cursor.blockNumber();			
	}
	m_skipModifications = false;
	return lastBlockNumber;
}

void CommentsEditor::replaceCurrentBlockText(QTextCursor &cursor, const QString &text) {

	m_skipModifications = true;
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.insertText(text);
	m_skipModifications = false;
}

void CommentsEditor::updateComments() {

	QTextCursor cursor(firstVisibleBlock());
	gotoBegin(cursor);

	do {
		int lineNumber = cursor.blockNumber();
		QString comment = m_projectFile->commentInLine(lineNumber);
		if (comment.isEmpty()) {
			replaceCurrentBlockText(cursor, "");
		}
		else {
			replaceCurrentBlockText(cursor, comment);
		}
	} while (cursor.movePosition(QTextCursor::NextBlock));
}

void CommentsEditor::saveComments() {

	if (m_isCommentsChanged) {
		QTextCursor cursor = textCursor();
		gotoBegin(cursor);

		QTextBlock block = cursor.block();

		while (block.isValid()) {
			int blockNumber = block.blockNumber();
			QString blockText = block.text();
			if (blockText.isEmpty()) {
				m_projectFile->removeCommentIfExists(blockNumber);
			}
			else {
				m_projectFile->addOrReplaceComment(blockNumber, blockText);
			}

			block = block.next();
		}
		emit applyCommentsSignal();
	}
}

void CommentsEditor::commentsChangedSlot() {

	if (!m_skipModifications && m_isInit) {
		qDebug() << sender()->metaObject()->className();
		m_isCommentsChanged = true;
		emit commentsChangedSignal();
	}
}

void CommentsEditor::scrolledSlot(int y) {

	if (!m_isLastUpdateRequestFromCode) {
		emit codeScrollRequestSignal(y);
	}

	m_isLastUpdateRequestFromCode = false;
}

void CommentsEditor::codePositionChangedSlot(int yPos) {

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
	m_isLastUpdateRequestFromCode = true;
	setTextCursor(cursor);
}