#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QMap>
#include <QString>
#include <QPlainTextEdit>

#include "ProjectFile.h"

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

  QWidget *m_leftArea;
  SProjectFile m_projectFile;

  bool m_isLastUpdateRequestFromComments;

  void loadProjectFile();
  void updateBreakPointAndComments();

protected:
  void resizeEvent(QResizeEvent *e);
  void focusInEvent(QFocusEvent *e);
  void focusOutEvent(QFocusEvent *e);

public:
  explicit CodeEditor(QWidget *parent = 0);

  void leftAreaPaintEvent(QPaintEvent *event);
  int leftAreaWidth();

  SProjectFile projectFile() const;
  void setProjectFile(SProjectFile projectFile);

signals:
  void commentsScrollRequestSignal(int y);
  void commentsUpdateRequestSignal(int blockCount);

private slots:
  void blockCountChangedSlot(int newBlockCount);
  void highlightCurrentLineSlot();
  void updateRequestSlot(const QRect &rect, int dy);
  void scrolledSlot(int y);

public slots:

};

#endif // CODEEDITOR_H
