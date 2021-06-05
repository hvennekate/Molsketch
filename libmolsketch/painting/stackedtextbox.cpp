#include "stackedtextbox.h"

#include <QPainter>

namespace Molsketch {

  QFont getSmallFont(QFont originalFont) {
    originalFont.setPointSizeF(originalFont.pointSizeF() * 0.6);
    return originalFont;
  }

  qreal getMiddle(QFontMetricsF metrics) {
    return 0.5 * (metrics.ascent() - metrics.descent());
  }

  StackedTextBox::StackedTextBox(const QString &topText, const QString &bottomText, const QPointF &offset, const QFont &font)
    : TextBox(offset, getSmallFont(font)),
      topText(topText),
      bottomText(bottomText),
      originalFontMetrics(font),
      shiftUp(-getMiddle(originalFontMetrics)),
      shiftDown(getMiddle(originalFontMetrics) - originalFontMetrics.descent())
  {}

  void StackedTextBox::render(QPainter *painter) const {
    painter->save();
    painter->setFont(font);
    painter->drawText(offset + QPointF(0, shiftDown), bottomText);
    painter->drawText(offset + QPointF(0, shiftUp), topText);
    painter->restore();
  }

  QRectF StackedTextBox::boundingRect() const {
    // TODO consider tight bounding box
    return metrics.boundingRect(bottomText).translated(0, shiftDown) |
        metrics.boundingRect(topText).translated(0, shiftUp);
  }

} // namespace Molsketch
