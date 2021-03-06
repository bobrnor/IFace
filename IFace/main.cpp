#include <QtGui/QApplication>
#include <QTextCodec>
#include <QLocale>
#include <QDebug>

#include "MainWindow.h"
#include "Project.h"
#include "GlobalState.h"

int main(int argc, char *argv[]) {

	setlocale(LC_ALL, "rus");
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("cp1251"));

    QApplication a(argc, argv);

	a.installTranslator(GlobalState::instance()->translator());
	if (GlobalState::instance()->langId()) {
		a.installTranslator(GlobalState::instance()->qtTranslator());
	}

    MainWindow w;
    w.show();

    return a.exec();
}
