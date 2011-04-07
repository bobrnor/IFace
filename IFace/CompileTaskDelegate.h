#pragma once

#include <QObject>
#include <QString>
#include "CompileResult.h"

class CompileTaskDelegate : public QObject {
Q_OBJECT

public slots:
	virtual void compileErrorSlot(QString command) = 0;
	virtual void compileDoneSlot(QString command, CompileResult *result = NULL) = 0;
};