#include "regulartextbox.h"
#include <QPainter>
#include <QDebug>

namespace Molsketch {

  QDebug RegularTextBox::debug(QDebug debug) const {
    return debug << "Regular text box(" << text << ", " << font << ")";
  }

  RegularTextBox::RegularTextBox(const QString &text, const QFont &font)
    : TextBox(font), text(text) {}

  QRectF RegularTextBox::boundingRect() const {
    // TODO consider tightBoundingRect() (possibly selectable by user)
    return metrics.boundingRect(text);
  }

  void RegularTextBox::paint(QPainter *painter) const {
    painter->save();
    painter->setFont(font);
    painter->drawText(0, 0, text);
    painter->restore();
  }

  bool RegularTextBox::preferredCenter() const {
    return true;
  }

} // namespace Molsketch
