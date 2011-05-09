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
				if (error.isValid()) {
					return error.yPos();
				}
				else {
					return "???";
				}				

			case 4:
				if (error.isValid()) {
					return error.xPos();
				}
				else {
					return "???";
				}

			case 5:
				return error.text();

			default:
				return QVariant();
		}
	}
	else if (role == Qt::CheckStateRole) {
		int column = index.column();
		if (column == 0) {
			return m_checks.at(index.row()) ? Qt::Checked : Qt::Unchecked;
		}
		else {
			return QVariant();
		}
	}
	else if (role == Qt::BackgroundRole) {
		int row = index.row();
		if (m_lastErrorRows.contains(row) && m_currentErrorRows.count() == 0 && m_currentCodeLineErrorRows.count() == 0) {
			return QBrush(QColor(244, 238, 224, 255));
		}
		else if (m_currentCodeLineErrorRows.contains(row)) {
			return QBrush(QColor(238, 233, 233, 255));
		}
		else if (m_currentErrorRows.contains(row)) {
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

bool ErrorTableModel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) {

	if (role == Qt::CheckStateRole) {
		m_checks.replace(index.row(), value.toBool());
		return true;
	}
	return false;
}

Qt::ItemFlags ErrorTableModel::flags(const QModelIndex &index) const {

	return (index.column() == 0) 
		? Qt::ItemIsUserCheckable |Qt::ItemIsEnabled | Qt::ItemIsSelectable 
		: Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ErrorTableModel::setErrorList(QList<CompileError> errorList) {

	m_lastErrorRows.clear();
	m_currentErrorRows.clear();
	m_currentCodeLineErrorRows.clear();
	m_errorList = errorList;

	m_checks.clear();
	for (int i = 0; i < m_errorList.count(); ++i) {
		m_checks.append(false);
	}

	reset();
}

void ErrorTableModel::updateErrorList(QList<CompileError> errorList) {
	
	m_errorList = errorList;
	update();
}

void ErrorTableModel::setCurrentErrorRows(QList<int> rows) {

	if (m_currentErrorRows.count() > 0) {
		m_lastErrorRows = m_currentErrorRows;
	}
	m_currentErrorRows = rows;
}

void ErrorTableModel::setCurrentCodeLineErrorRows(QList<int> errorRows) {

	m_currentCodeLineErrorRows = errorRows;
}

void ErrorTableModel::update() {

	emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void ErrorTableModel::setCheckStatusForAll(bool checked) {

	for (int i = 0; i < m_checks.count(); ++i) {
		m_checks[i] = checked;
	}
}