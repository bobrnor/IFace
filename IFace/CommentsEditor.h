#pragma once 

#include <QMap>
#include <QString>
#include <QPlainTextEdit>

#include "ProjectFile.h"

class CommentsEditor : public QPlainTextEdit {
	Q_OBJECT

	ProjectFile *m_projectFile;

	int m_codeBlockCount;
	bool m_isCommentsChanged;
	bool m_changingBlockCount;
	bool m_isLastUpdateRequestFromCode;
	bool m_isInit;
	bool m_skipModifications;

	void gotoBegin(QTextCursor &cursor);
	int gotoEnd(QTextCursor &cursor);
	void replaceCurrentBlockText(QTextCursor &cursor, const QString &text);
	void makeProperLineCount(int lineCount);
	void updateComments();
	void saveComments();

protected:
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);

public:
	CommentsEditor(QWidget *parent = 0);
	~CommentsEditor();

	ProjectFile *projectFile() const { return m_projectFile; }
	void setProjectFile(ProjectFile *projectFile);

private slots:
	void highlightCurrentLineSlot();
	void blockCountChangedSlot(int newBlockCount);
	void commentsChangedSlot();

public slots:
	void scrolledSlot(int y);
	void codeScrolledSlot(int y);
	void codeBlockCountChangedSlot(int newBlockCount);
	void codePositionChangedSlot(int yPos);

signals:
	void applyCommentsSignal();
	void codeScrollRequestSignal(int y);	
	void commentsCursorLineChangedSignal(int yPos);
	void commentsChangedSignal();
};