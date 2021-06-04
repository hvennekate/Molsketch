#include "regulartextbox.h"
#include <QPainter>

namespace Molsketch {

RegularTextBox::RegularTextBox(const QString &text, const QPointF &offset, const QFont &font)
  : TextBox(offset, font), text(text) {}

QRectF RegularTextBox::boundingRect() const {
  return metrics.boundingRect(text);
}

void RegularTextBox::render(QPainter *painter) const {
  painter->save();
  painter->setFont(font);
  painter->drawText(offset, text);
  painter->restore();
}

} // namespace Molsketch
