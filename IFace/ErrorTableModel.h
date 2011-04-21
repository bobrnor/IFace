#pragma once

#include <QAbstractItemModel>
#include <QList>

#include "CompileError.h"

class ErrorTableModel : public QAbstractTableModel {

	QList<CompileError> m_errorList;
	int m_lastErrorRow;
	int m_currentErrorRow;
	QList<int> m_currentCodeLineErrorRows;

private:

public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void setErrorList(QList<CompileError> errorList);
	QList<CompileError> errorList() { return m_errorList; }

	void setCurrentErrorRow(int row);
	int currentErrorRow() { return m_currentErrorRow; }

	void setCurrentCodeLineErrorRows(QList<int> errorRows);
	QList<int> currentCodeLineErrorRows() { return m_currentCodeLineErrorRows; }

	void update();
};