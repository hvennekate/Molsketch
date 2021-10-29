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

  StackedTextBox::StackedTextBox(const QString &topText, const QString &bottomText, const QFont &font)
    : TextBox(getSmallFont(font)),
      topText(topText),
      bottomText(bottomText),
      originalFontMetrics(font),
      shiftUp(-getMiddle(originalFontMetrics)),
      shiftDown(getMiddle(originalFontMetrics) - originalFontMetrics.descent())
  {}

  void StackedTextBox::paint(QPainter *painter) const {
    painter->save();
    painter->setFont(font);
    painter->drawText(QPointF(0, shiftDown), bottomText);
    painter->drawText(QPointF(0, shiftUp), topText);
    painter->restore();
  }

  QRectF StackedTextBox::boundingRect() const {
    // TODO consider tight bounding box
    return metrics.boundingRect(bottomText).translated(0, shiftDown) |
        metrics.boundingRect(topText).translated(0, shiftUp);
  }

  bool StackedTextBox::preferredCenter() const {
    return false;
  }

} // namespace Molsketch
