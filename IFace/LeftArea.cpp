#include "LeftArea.h"

#include <QVBoxLayout>

#include "CodeEditor.h"

LeftArea::LeftArea(CodeEditor *codeEditor) : QWidget(codeEditor) {

  m_codeEditor = codeEditor;
  setLayout(new QVBoxLayout(this));
}

QSize LeftArea::sizeHint() const {

  return QSize(m_codeEditor->leftAreaWidth(), 0);
}

void LeftArea::paintEvent(QPaintEvent *event) {

  m_codeEditor->leftAreaPaintEvent(event);
}
