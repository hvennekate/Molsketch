#ifndef MOLSKETCH_REGULARTEXTBOX_H
#define MOLSKETCH_REGULARTEXTBOX_H

#include <QFont>
#include <QPointF>
#include <QString>
#include "textbox.h"

namespace Molsketch {

class RegularTextBox : public TextBox {
  QString text;
public:
  RegularTextBox(const QString &text, const QFont &font);

  QRectF boundingRect() const override;
  void paint(QPainter *painter) const override;
  bool preferredCenter() const override;
};


} // namespace Molsketch

#endif // MOLSKETCH_REGULARTEXTBOX_H
