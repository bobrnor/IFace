#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

#include "ProjectFile.h"

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

  QWidget *m_leftArea;
  SProjectFile m_projectFile;

  void loadProjectFile();

protected:
  void resizeEvent(QResizeEvent *e);

public:
  explicit CodeEditor(QWidget *parent = 0);

  void leftAreaPaintEvent(QPaintEvent *event);
  int leftAreaWidth();

  SProjectFile projectFile() const;
  void setProjectFile(SProjectFile projectFile);

signals:

private slots:
  void updateLeftAreaWidthSlot(int newBlockCount);
  void highlightCurrentLineSlot();
  void updateLeftAreaSlot(const QRect &rect, int dy);

public slots:

};

#endif // CODEEDITOR_H
