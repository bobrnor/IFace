#include "CodeLineData.h"

CodeLineData::CodeLineData() {

	hasBreakPoint = false;
	comment = "";
}

CodeLineData *CodeLineData::copy() {

	CodeLineData *result = new CodeLineData();
	result->hasBreakPoint = hasBreakPoint;
	result->comment = comment;
	return result;
}