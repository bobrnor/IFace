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

		QList<int> currentErrorRows;
		QList<int> currentCodeLineErrors;

		int i = 0;
		int line = currentError.yPos() - 1;
		int character = currentError.xPos() - 1;

		foreach (CompileError error, m_currentErrorList) {
			if (!error.isValid()) {
				continue;
			}
			if (currentError.projectFile()->path() == error.projectFile()->path()) {
				if (line == error.yPos() - 1) {
					if (character == error.xPos() - 1) {
						currentErrorRows.append(i);
					}
					else {
						currentCodeLineErrors.append(i);
					}
				}
			}
			i++;
		}

		m_errorTableModel->setCurrentErrorRows(currentErrorRows);
		m_errorTableModel->setCurrentCodeLineErrorRows(currentCodeLineErrors);
		m_errorTableModel->update();
	}
	m_isLastUpdateRequestFromCodeEditor = false;
}

void ErrorTableHelper::codeCursorChangedSlot(ProjectFile *file, int xPos, int yPos) {

	QList<int> currentErrorRows;
	QList<int> currentCodeLineErrors;

	int i = 0;
	foreach (CompileError error, m_currentErrorList) {
		if (!error.isValid()) {
			continue;
		}
		QString errorFilePath = error.projectFile()->path();
		if (errorFilePath == file->path()) {
			int line = error.yPos() - 1;
			int character = error.xPos() - 1;

			if (line == yPos) {
				if (character == xPos) {
					currentErrorRows.append(i);
				}
				else {
					currentCodeLineErrors.append(i);
				}
			}
		}
		i++;
	}

	m_errorTableModel->setCurrentErrorRows(currentErrorRows);
	m_errorTableModel->setCurrentCodeLineErrorRows(currentCodeLineErrors);
	m_errorTableModel->update();

	if (currentErrorRows.count() > 0) {
		m_isLastUpdateRequestFromCodeEditor = true;
		m_linkedTable->selectRow(currentErrorRows.at(0));
	}
}

void ErrorTableHelper::codeChangedSlot(ProjectFile *file, int xPos, int yPos) {

	QString sourceFilePath = file->path();

	for (int i = 0; i < m_currentErrorList.count(); ++i) {
		CompileError error = m_currentErrorList.at(i);
		QString errorFilePath = error.projectFile()->path();
		if (sourceFilePath == errorFilePath) {
			int line = error.yPos() - 1;
			int character = error.xPos() - 1;
			if ((line == yPos && character >= xPos) || (line > yPos)) {
// 				QList<CompileError> errors = file->compileErrorsRu();
// 				CompileError e = errors.at(i);
// 				e.setIsValid(false);
// 				errors.replace(i, e);
// 				file->setCompileErrorsRu(errors);
// 
// 				errors = file->compileErrorsEn();
// 				e = errors.at(i);
// 				e.setIsValid(false);
// 				errors.replace(i, e);
// 				file->setCompileErrorsEn(errors);

				error.setIsValid(false);
				m_currentErrorList.replace(i, error);
			}
		}
	}

	m_errorTableModel->updateErrorList(m_currentErrorList);
}