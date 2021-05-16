#include "regulartextbox.h"
#include <QPainter>

namespace Molsketch {

RegularTextBox::RegularTextBox(const QString &text, const QPointF &offset, const QFont &font)
  : TextBox(offset, font), text(text) {}

QRectF RegularTextBox::boundingRect() const {
  return QRectF(offset, QSizeF(metrics.width(text), metrics.height()))
      .translated(0, metrics.descent()-metrics.height()); // TODO replace with metrics.boundingRect()
  //      return QRectF(metrics.boundingRect(text)).translated(offset); // TODO remove QRectF with QFontMetricsF
}

void RegularTextBox::render(QPainter *painter) const {
  painter->save();
  painter->setFont(font);
  painter->drawText(offset, text);
  painter->restore();
}

} // namespace Molsketch
