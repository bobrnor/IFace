#include <QtGui/QApplication>
#include <QTextCodec>

#include "MainWindow.h"
#include "Project.h"

int main(int argc, char *argv[]) {

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
