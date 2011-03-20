#pragma once

#include <QWidget>

class CodeEditor;
class CommentsArea;

namespace Ui {
	class CodeEditorWidget;
}

class CodeEditorWidget : public QWidget {
	Q_OBJECT

	Ui::CodeEditorWidget *ui;

	CodeEditor *m_codeEditor;
	CommentsArea *m_commentsArea;

protected:

public:
	CodeEditorWidget(QWidget *parent);
	~CodeEditorWidget();

	CodeEditor *codeEditor() const;
	void setCodeEditor(CodeEditor *codeEditor);
	CommentsArea *commentsArea() const;
	void setCommentsArea(CommentsArea *commentsArea);

};