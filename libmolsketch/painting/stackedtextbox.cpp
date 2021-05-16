#include "stackedtextbox.h"

#include <QPainter>

namespace Molsketch {

StackedTextBox::StackedTextBox(const QString &topText, const QString &bottomText, const QPointF &offset, const QFont &font, const QFontMetricsF &mainFontMetrics)
  : TextBox(offset, font),
    topText(topText),
    bottomText(bottomText),
    shiftUp(mainFontMetrics.ascent()),
    shiftDown(mainFontMetrics.descent()),
    mainHeight(mainFontMetrics.height())
{}

void StackedTextBox::render(QPainter *painter) const {
  painter->save();
  painter->setFont(font);
  painter->drawText(offset + QPointF(0, shiftDown), bottomText);
  painter->drawText(offset + QPointF(0, shiftUp), topText);
  painter->restore();
}

QRectF StackedTextBox::boundingRect() const {
  return QRectF(offset, QSizeF(qMax(metrics.width(topText), metrics.width(bottomText)), mainHeight))
      .translated(0, shiftDown - mainHeight);
  // TODO compute height correctly
}

} // namespace Molsketch
