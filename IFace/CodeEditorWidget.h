#pragma once

#include <QWidget>

class CodeEditor;
class CommentsEditor;

namespace Ui {
	class CodeEditorWidget;
}

class CodeEditorWidget : public QWidget {
	Q_OBJECT

	Ui::CodeEditorWidget *ui;

	CodeEditor *m_codeEditor;
	CommentsEditor *m_commentsArea;

protected:
	void linkCodeWithComments();

public:
	CodeEditorWidget(QWidget *parent);
	~CodeEditorWidget();

	CodeEditor *codeEditor() const;
	void setCodeEditor(CodeEditor *codeEditor);
	CommentsEditor *commentsArea() const;
	void setCommentsArea(CommentsEditor *commentsArea);

};