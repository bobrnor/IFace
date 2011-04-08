#pragma once

#include <QObject>
#include <QString>
#include "CompileResult.h"

class CompileTaskDelegate : public QObject {
Q_OBJECT

public slots:
	virtual void compileErrorSlot(uint key) = 0;
	virtual void compileDoneSlot(uint key) = 0;
};