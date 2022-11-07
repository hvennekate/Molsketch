#ifndef MOLSKETCH_REGULARTEXTBOX_H
#define MOLSKETCH_REGULARTEXTBOX_H

#include <QFont>
#include <QPointF>
#include <QString>
#include "textbox.h"
#include "debuggable.h"

namespace Molsketch {

class RegularTextBox : public TextBox {
  QString text;
  QDebug debug(QDebug debug) const override;
public:
  RegularTextBox(const QString &text, const QFont &font);

  QRectF boundingRect() const override;
  void paint(QPainter *painter) const override;
  bool preferredCenter() const override;
};

} // namespace Molsketch

#endif // MOLSKETCH_REGULARTEXTBOX_H
