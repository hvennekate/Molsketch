#include "stackedtextbox.h"

#include <QPainter>

namespace Molsketch {

  QFont getSmallFont(QFont originalFont) {
    originalFont.setPointSizeF(originalFont.pointSizeF()/2.-1.);
    return originalFont;
  }

  StackedTextBox::StackedTextBox(const QString &topText, const QString &bottomText, const QPointF &offset, const QFont &font)
    : TextBox(offset, getSmallFont(font)),
      topText(topText),
      bottomText(bottomText),
      originalFontMetrics(font)
  {}

  void StackedTextBox::render(QPainter *painter) const {
    painter->save();
    painter->setFont(font);
    qreal shiftDown = -10, shiftUp = 10;
    painter->drawText(offset + QPointF(0, shiftDown), bottomText);
    painter->drawText(offset + QPointF(0, shiftUp), topText);
    painter->restore();
  }

  QRectF StackedTextBox::boundingRect() const {
    qreal width = std::max(metrics.width(topText), metrics.width(bottomText));
    return QRectF(QPointF(-width/2., originalFontMetrics.ascent()),
                  QSizeF(width, originalFontMetrics.ascent() + originalFontMetrics.descent()));
  }

} // namespace Molsketch
