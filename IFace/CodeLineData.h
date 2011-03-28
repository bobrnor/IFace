#pragma once

#include <QString>
#include <QTextBlockUserData>

class CodeLineData : public QTextBlockUserData {

public:
	bool hasBreakPoint;
	QString comment;
};