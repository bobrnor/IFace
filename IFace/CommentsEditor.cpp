#include "CommentsEditor.h"

#include <QDebug>

#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>

CommentsEditor::CommentsEditor(QWidget *parent /* = 0 */) : QPlainTextEdit(parent) {

	qDebug() << __FUNCSIG__;
	m_codeBlockCount = 0;

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLineSlot()));

	highlightCurrentLineSlot();
}

CommentsEditor::~CommentsEditor() {

	qDebug() << __FUNCSIG__;
}

void CommentsEditor::setProjectFile(SProjectFile projectFile) {

	qDebug() << __FUNCSIG__;
	m_projectFile = projectFile;
}

void CommentsEditor::highlightCurrentLineSlot() {

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
	}

	setExtraSelections(extraSelections);
}

void CommentsEditor::focusInEvent(QFocusEvent *e) {

	qDebug() << __FUNCSIG__;
	highlightCurrentLineSlot();
}

void CommentsEditor::focusOutEvent(QFocusEvent *e) {

	qDebug() << __FUNCSIG__;
	highlightCurrentLineSlot();
}

void CommentsEditor::codeScrolledSlot(int y) {

	qDebug() << __FUNCSIG__;
	verticalScrollBar()->setValue(y);
}

void CommentsEditor::codeBlockCountChangedSlot(int newBlockCount) {

	qDebug() << __FUNCSIG__;

	if (newBlockCount > 0) {
		QTextCursor cursor = textCursor();

		int lastBlockNumber = cursor.blockNumber();
		while (cursor.movePosition(QTextCursor::NextBlock)) {
			lastBlockNumber = cursor.blockNumber();			
		}

		if (lastBlockNumber != newBlockCount - 1) {
			if (lastBlockNumber > newBlockCount - 1) {
				int delta = lastBlockNumber - newBlockCount + 1;
				cursor.movePosition(QTextCursor::EndOfBlock);
				cursor.movePosition(QTextCursor.PreviousBlock, QTextCursor::KeepAnchor, delta);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
			}
			else {
				int delta = newBlockCount - lastBlockNumber - 1;
				for (int i = 0; i < delta; ++i) {
					cursor.insertBlock();
				}
			}
		}
	}
}