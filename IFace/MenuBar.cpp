#include "MenuBar.h"

void MenuBar::leaveEvent(QEvent *event) {

	emit leaveSignal();
	QMenuBar::leaveEvent(event);
}