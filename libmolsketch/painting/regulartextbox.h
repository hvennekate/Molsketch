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
  RegularTextBox(const QString &text, const QPointF &offset, const QFont &font);

  QRectF boundingRect() const override;
  void render(QPainter *painter) const override;
};


} // namespace Molsketch

#endif // MOLSKETCH_REGULARTEXTBOX_H
