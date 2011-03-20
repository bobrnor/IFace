#pragma once

#include <QPlainTextEdit>


class CommentsArea : public QPlainTextEdit {
	Q_OBJECT

protected:

public:
	CommentsArea(QWidget *parent);

	void insertBlankLine(int position);
	void removeLine(int position);
};