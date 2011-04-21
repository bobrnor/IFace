#include "ErrorTableModel.h"

#include <QDebug>

#include "ProjectFile.h"

int ErrorTableModel::rowCount(const QModelIndex &parent /* = QModelIndex */) const {

	qDebug() << __FUNCSIG__;
	return m_errorList.count();
}

int ErrorTableModel::columnCount(const QModelIndex &parent /* = QModelIndex */) const {

	qDebug() << __FUNCSIG__;
	return 6;
}

QVariant ErrorTableModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const {

	qDebug() << __FUNCSIG__;
	if (!index.isValid()) {
		return QVariant();
	}

	if (role == Qt::TextAlignmentRole) {
		return Qt::AlignLeft;
	}
	else if (role == Qt::DisplayRole) {
		int row = index.row();
		int column = index.column();
		CompileError error = m_errorList.at(row);

		switch (column) {
			case 1:
				return row;

			case 2:
				return error.projectFile()->path();

			case 3:
				return error.yPos();

			case 4:
				return error.xPos();

			case 5:
				return error.text();

			default:
				return QVariant();
		}
	}
	else if (role == Qt::CheckStateRole) {
		int row = index.row();
		int column = index.column();
		if (column == 0) {
			return Qt::Unchecked;
		}
		else {
			return QVariant();
		}
	}
	else {
		return QVariant();
	}
}

void ErrorTableModel::setErrorList(QList<CompileError> errorList) {

	qDebug() << __FUNCSIG__;
	beginResetModel();
	m_errorList = errorList;
	endResetModel();
}