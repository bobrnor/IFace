#pragma once

#include <QAbstractItemModel>
#include <QList>

#include "CompileError.h"

class ErrorTableModel : public QAbstractTableModel {

	QList<CompileError> m_errorList;

private:

public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setErrorList(QList<CompileError> errorList);
};