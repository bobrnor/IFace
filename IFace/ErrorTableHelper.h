#pragma once

#include <QObject>
#include <QTableView>
#include <QList>

#include "CompileError.h"
#include "ProjectFile.h"
#include "ErrorTableModel.h" 

class ErrorTableHelper : public QObject {
	Q_OBJECT

	QTableView *m_linkedTable;
	ErrorTableModel *m_errorTableModel;
	QList<CompileError> m_currentErrorList;
	QList<CompileError> m_ruErrorList;
	QList<CompileError> m_enErrorList;
	int m_lastErrorRow;

	QList<QShortcut*> m_shortcutList;

	bool m_isLastUpdateRequestFromCodeEditor;

	CompileError m_currentEnError;
	CompileError m_currentRuError;

	void processErrorList();

public:
	ErrorTableHelper();
	~ErrorTableHelper();

	void setErrorTable(QTableView *tableView);
	QTableView *errorTable() { return m_linkedTable; }

	CompileError currentEnError() { return m_currentEnError; }
	CompileError currentRuError() { return m_currentRuError; }

	void setErrorLists(QList<CompileError> enErrorList, QList<CompileError> ruErrorList);
	QList<CompileError> errorRuList() { return m_ruErrorList; }
	QList<CompileError> errorEnList() { return m_enErrorList; }

	void retranslate();

private slots:
	void currentTableRowChangedSlot(const QModelIndex & current, const QModelIndex & previous);

public slots:
	void codeCursorChangedSlot(ProjectFile *file, int xPos, int yPos);
	void codeChangedSlot(ProjectFile *file, int xPos, int yPos);
	void checkAllSlot();
	void uncheckAllSlot();
	void tableContextMenuRequestSlot(const QPoint &pos);

signals:
	void errorPositionChangedSignal(ProjectFile *file, int xPos, int yPos);
};