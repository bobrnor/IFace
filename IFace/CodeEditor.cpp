#include "CodeEditor.h"

#include <cmath>
#include <QDebug>
#include <QTextEdit>
#include <QList>
#include <QFile>
#include <QColor>
#include <QPainter>
#include <QTextBlock>
#include <QVBoxLayout>

#include <LeftArea.h>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {

  setWordWrapMode(QTextOption::NoWrap);
  setLayout(new QVBoxLayout(this));

  m_leftArea = new LeftArea(this);

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLeftAreaWidthSlot(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLeftAreaSlot(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLineSlot()));

  updateLeftAreaWidthSlot(0);
  highlightCurrentLineSlot();
}

int CodeEditor::leftAreaWidth() {

  int max = qMax(1, blockCount());
  int digits = 1 + log10(max);
  qDebug() << "Digit count: " << digits;

  return 3 + fontMetrics().width(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLeftAreaWidthSlot(int newBlockCount) {

  setViewportMargins(leftAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLeftAreaSlot(const QRect &rect, int dy) {

  if (dy) {
    m_leftArea->scroll(0, dy);
  }
  else {
    m_leftArea->update(0, rect.y(), leftAreaWidth(), rect.height());
  }

  if (rect.contains(viewport()->rect())) {
    updateLeftAreaWidthSlot(0);
  }
}

void CodeEditor::resizeEvent(QResizeEvent *e) {

  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  m_leftArea->setGeometry(cr.left(), cr.top(), leftAreaWidth(), cr.height());
}

void CodeEditor::highlightCurrentLineSlot() {

  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void CodeEditor::leftAreaPaintEvent(QPaintEvent *event) {

  QPainter painter(m_leftArea);
  painter.fillRect(event->rect(), Qt::lightGray);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
        QString number = QString::number(blockNumber + 1);
        painter.setPen(Qt::black);
        painter.drawText(0, top, m_leftArea->width(), fontMetrics().height(),
                         Qt::AlignRight, number);
      }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

ProjectFile *CodeEditor::projectFile() const {

  return m_projectFile;
}

void CodeEditor::setProjectFile(ProjectFile *projectFile) {

  m_projectFile = projectFile;
  loadProjectFile();
}

void CodeEditor::loadProjectFile() {

  if (m_projectFile != NULL) {

    QFile file(m_projectFile->path());
    if (file.open(QIODevice::ReadOnly)) {

      QTextStream stream(&file);
      while (!stream.atEnd()) {
        QString codeline = stream.readLine();
        insertPlainText(codeline);
      }
    }
  }
}
