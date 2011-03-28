#pragma once 

#include <QMap>
#include <QString>
#include <QPlainTextEdit>

#include "ProjectFile.h"

class CommentsEditor : public QPlainTextEdit {
	Q_OBJECT

	SProjectFile m_projectFile;

	int m_codeBlockCount;

	void gotoBegin(QTextCursor &cursor);
	int gotoEnd(QTextCursor &cursor);
	void replaceCurrentBlockText(QTextCursor &cursor, const QString &text);
	void updateComments();

protected:
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);

public:
	CommentsEditor(QWidget *parent = 0);
	~CommentsEditor();

	SProjectFile projectFile() const { return m_projectFile; }
	void setProjectFile(SProjectFile projectFile);

private slots:
	void highlightCurrentLineSlot();
	void codeScrolledSlot(int y);
	void codeBlockCountChangedSlot(int newBlockCount);

};