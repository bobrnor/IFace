#pragma once

#include <QMenuBar>
#include <QWidget>

class MenuBar : public QMenuBar {
Q_OBJECT

protected:
	void leaveEvent(QEvent *event);

public:
	MenuBar(QWidget *parent = NULL) : QMenuBar(parent) {}

signals:
	void leaveSignal();
};