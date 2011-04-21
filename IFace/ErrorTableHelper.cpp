#include "ErrorTableHelper.h"

ErrorTableHelper::ErrorTableHelper() {

	m_errorTableModel = new ErrorTableModel();
	m_isLastUpdateRequestFromCodeEditor = false;
}

ErrorTableHelper::~ErrorTableHelper() {

	delete m_errorTableModel;
}

void ErrorTableHelper::setErrorTable(QTableView *tableView) {

	m_linkedTable = tableView;
	m_lastErrorRow = -1;
	if (m_linkedTable != NULL) {
		m_linkedTable->setModel(m_errorTableModel);
		if (m_linkedTable->selectionModel() != NULL) {
			connect(m_linkedTable->selectionModel(), SIGNAL(currentRowChanged (const QModelIndex&, const QModelIndex&)), 
				this, SLOT(currentTableRowChangedSlot(const QModelIndex&, const QModelIndex&)));
		}
	}
}

void ErrorTableHelper::setErrorLists(QList<CompileError> ruErrorList, QList<CompileError> enErrorList) {

	m_ruErrorList = ruErrorList;
	m_enErrorList = enErrorList;
	m_currentErrorList = m_ruErrorList;
	m_lastErrorRow = -1;
	m_errorTableModel->setErrorList(m_currentErrorList);
}

void ErrorTableHelper::currentTableRowChangedSlot(const QModelIndex &current, const QModelIndex &previous) {

	int row = current.row();
	if (!m_isLastUpdateRequestFromCodeEditor && row < m_currentErrorList.count()) {
		CompileError currentError = m_currentErrorList.at(row);
		emit errorPositionChangedSignal(currentError.projectFile(), currentError.xPos(), currentError.yPos());
	}
	m_isLastUpdateRequestFromCodeEditor = false;
}

void ErrorTableHelper::codeCursorChangedSlot(ProjectFile *file, int xPos, int yPos) {

	int currentErrorRow = -1;
	QList<int> currentCodeLineErrors;

	int i = 0;
	foreach (CompileError error, m_currentErrorList) {
		QString errorFilePath = error.projectFile()->path();
		if (errorFilePath == file->path()) {
			int line = error.yPos() - 1;
			int character = error.xPos() - 1;

			if (line == yPos) {
				if (character == xPos) {
					currentErrorRow = i;
				}
				else {
					currentCodeLineErrors.append(i);
				}
			}
		}
		i++;
	}

	m_errorTableModel->setCurrentErrorRow(currentErrorRow);
	m_errorTableModel->setCurrentCodeLineErrorRows(currentCodeLineErrors);
	m_errorTableModel->update();

	if (currentErrorRow >= 0) {
		m_isLastUpdateRequestFromCodeEditor = true;
		m_linkedTable->selectRow(currentErrorRow);
	}
}