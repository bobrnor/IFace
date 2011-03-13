#include "LeftArea.h"

#include "CodeEditor.h"

LeftArea::LeftArea(CodeEditor *codeEditor) : QWidget(codeEditor) {

  m_codeEditor = codeEditor;
}

QSize LeftArea::sizeHint() const {

  return QSize(m_codeEditor->leftAreaWidth(), 0);
}

void LeftArea::paintEvent(QPaintEvent *event) {

  m_codeEditor->leftAreaPaintEvent(event);
}
