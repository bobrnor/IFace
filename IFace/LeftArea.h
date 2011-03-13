#ifndef LEFTAREA_H
#define LEFTAREA_H

#include <QWidget>

class CodeEditor;

class LeftArea : public QWidget {
  Q_OBJECT

  CodeEditor *m_codeEditor;

protected:
  void paintEvent(QPaintEvent *event);

public:
  explicit LeftArea(CodeEditor *codeEditor);

  QSize sizeHint() const;

signals:

public slots:

};

#endif // LEFTAREA_H
