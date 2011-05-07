#pragma once

#include <QAbstractItemModel>
#include <QList>

#include "CompileError.h"

class ErrorTableModel : public QAbstractTableModel {

	QList<bool> m_checks;
	QList<CompileError> m_errorList;
	QList<int> m_lastErrorRows;
	QList<int> m_currentErrorRows;
	QList<int> m_currentCodeLineErrorRows;

private:

public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	Qt::ItemFlags flags(const QModelIndex &index) const;

	void updateErrorList(QList<CompileError> errorList);
	void setErrorList(QList<CompileError> errorList);
	QList<CompileError> errorList() { return m_errorList; }

	void setCurrentErrorRows(QList<int> rows);
	QList<int> currentErrorRows() { return m_currentErrorRows; }

	void setCurrentCodeLineErrorRows(QList<int> errorRows);
	QList<int> currentCodeLineErrorRows() { return m_currentCodeLineErrorRows; }

	void update();
};