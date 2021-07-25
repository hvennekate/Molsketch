#include "regulartextbox.h"
#include <QPainter>

namespace Molsketch {

RegularTextBox::RegularTextBox(const QString &text, const QPointF &offset, const QFont &font)
  : TextBox(offset, font), text(text) {}

QRectF RegularTextBox::boundingRect() const {
  // TODO consider tightBoundingRect() (possibly selectable by user
  return metrics.boundingRect(text);
}

void RegularTextBox::paint(QPainter *painter) const {
  painter->save();
  painter->setFont(font);
  painter->drawText(offset, text);
  painter->restore();
}

} // namespace Molsketch
