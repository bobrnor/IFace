#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QMap>
#include <QString>
#include <QPlainTextEdit>
#include <QTemporaryFile>

#include "ProjectFile.h"

class CodeEditor : public QPlainTextEdit {
	Q_OBJECT

	QWidget *m_leftArea;
	ProjectFile *m_projectFile;

	int m_currentXErrorPosition;
	int m_currentYErrorPosition;

	QList<QTextEdit::ExtraSelection> m_errorSymbols;
	QList<QTextEdit::ExtraSelection> m_errorSelections;

	bool m_isInit;
	bool m_isLastUpdateRequestFromComments;
	bool m_isLastUpdateRequestFromErrorTable;
	int m_lastCommentOffsetLine;

	bool m_isChanged;

	void loadProjectFile();

	void initCommentsAndBreakPoints();
	void updateBreakPointAndComments();
	QTextBlock blockWithNumber(int blockNumber);
	void moveDownComments(int fromBlockNumber);
	void updateErrors();

protected:
	void resizeEvent(QResizeEvent *e);
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void inputMethodEvent(QInputMethodEvent *event);

public:
	explicit CodeEditor(QWidget *parent = 0);
	~CodeEditor();

	void saveProjectFile();
	void tempSaveProjectFile();

	void leftAreaPaintEvent(QPaintEvent *event);
	int leftAreaWidth();

	ProjectFile *projectFile() const;
	void setProjectFile(ProjectFile *projectFile);

	void setIsChanged(bool changed) { m_isChanged = changed; }
	bool isChanged() { return m_isChanged; }

signals:
	void commentsScrollRequestSignal(int y);
	void commentsUpdateRequestSignal(int blockCount);
	void codeCursorLineChangedSignal(int yPos);
	void codeCursorChangedSignal(ProjectFile *file, int xPos, int yPos);
	void codeChangedSignal(ProjectFile *file, int xPos, int yPos);

private slots:
	void blockCountChangedSlot(int newBlockCount);
	void highlightCurrentLineSlot();
	void updateRequestSlot(const QRect &rect, int dy);
	void scrolledSlot(int y);

public slots:
	void applyCommentsSlot();
	void commentsScrolledSlot(int y);
	void commentsPositionChangedSlot(int yPos);
	void errorPositionChangedSlot(int xPos, int yPos);
	void textChangedSlot();
};

#endif // CODEEDITOR_H
