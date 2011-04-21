#include "ErrorTableModel.h"

#include <QDebug>
#include <QBrush>
#include <QColor>

#include "ProjectFile.h"

int ErrorTableModel::rowCount(const QModelIndex &parent /* = QModelIndex */) const {

	return m_errorList.count();
}

int ErrorTableModel::columnCount(const QModelIndex &parent /* = QModelIndex */) const {

	return 6;
}

QVariant ErrorTableModel::data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const {

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
				return row + 1;

			case 2:
				return error.projectFile()->fileName();

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
		int column = index.column();
		if (column == 0) {
			return Qt::Unchecked;
		}
		else {
			return QVariant();
		}
	}
	else if (role == Qt::BackgroundRole) {
		int row = index.row();
		if (row == m_lastErrorRow && m_currentErrorRow < 0) {
			return QBrush(QColor(244, 238, 224, 255));
		}
		else if (m_currentCodeLineErrorRows.contains(row)) {
			return QBrush(QColor(238, 233, 233, 255));
		}
		else if (m_currentErrorRow == row) {
			return QBrush(QColor(205, 201, 201, 255));
		}
		else {
			return QVariant();
		}
	}
	else {
		return QVariant();
	}
}

QVariant ErrorTableModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole */) const {

	if (orientation == Qt::Vertical) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		switch (section) {
			case 0:
				return QVariant();
				break;

			case 1:
				return "Error No";
				break;

			case 2:
				return "File name";
				break;

			case 3:
				return "Line";
				break;

			case 4:
				return "Character";
				break;

			case 5:
				return "Error text";
				break;
		}
	}
	return QVariant();
}

void ErrorTableModel::setErrorList(QList<CompileError> errorList) {

	qDebug() << __FUNCSIG__;
	beginResetModel();
	m_lastErrorRow = -1;
	m_currentErrorRow = -1;
	m_currentCodeLineErrorRows.clear();
	m_errorList = errorList;
	endResetModel();
}

void ErrorTableModel::setCurrentErrorRow(int row) {

	if (m_currentErrorRow >= 0) {
		m_lastErrorRow = m_currentErrorRow;
	}
	m_currentErrorRow = row;
}

void ErrorTableModel::setCurrentCodeLineErrorRows(QList<int> errorRows) {

	m_currentCodeLineErrorRows = errorRows;
}

void ErrorTableModel::update() {

	beginResetModel();
	endResetModel();
}