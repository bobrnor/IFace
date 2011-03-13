#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

  QWidget *m_leftArea;

protected:
  void resizeEvent(QResizeEvent *e);

public:
  explicit CodeEditor(QWidget *parent = 0);

  void leftAreaPaintEvent(QPaintEvent *event);
  int leftAreaWidth();

signals:

private slots:
  void updateLeftAreaWidthSlot(int newBlockCount);
  void highlightCurrentLineSlot();
  void updateLeftAreaSlot(const QRect &rect, int dy);

public slots:

};

#endif // CODEEDITOR_H
