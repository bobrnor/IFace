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
	SProjectFile m_projectFile;

	bool m_isInit;
	bool m_isLastUpdateRequestFromComments;
	int m_lastCommentOffsetLine;

	QTemporaryFile *m_tempFile;

	void loadProjectFile();

	void initCommentsAndBreakPoints();
	void updateBreakPointAndComments();
	QTextBlock blockWithNumber(int blockNumber);
	void moveDownComments(int fromBlockNumber);

protected:
	void resizeEvent(QResizeEvent *e);
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);
	void keyPressEvent(QKeyEvent *e);

public:
	explicit CodeEditor(QWidget *parent = 0);
	~CodeEditor();

	void saveProjectFile();
	void tempSaveProjectFile();

	void leftAreaPaintEvent(QPaintEvent *event);
	int leftAreaWidth();

	SProjectFile projectFile() const;
	void setProjectFile(SProjectFile projectFile);

signals:
	void commentsScrollRequestSignal(int y);
	void commentsUpdateRequestSignal(int blockCount);
	void codeCursorLineChangedSignal(int yPos);

private slots:
	void blockCountChangedSlot(int newBlockCount);
	void highlightCurrentLineSlot();
	void updateRequestSlot(const QRect &rect, int dy);
	void scrolledSlot(int y);

public slots:
	void applyCommentsSlot();
	void commentsScrolledSlot(int y);
	void commentsPositionChangedSlot(int yPos);
};

#endif // CODEEDITOR_H
