#ifndef MOLSKETCH_STACKEDTEXTBOX_H
#define MOLSKETCH_STACKEDTEXTBOX_H

#include "textbox.h"

namespace Molsketch {

class StackedTextBox : public TextBox {
  QString topText, bottomText;
  QFontMetricsF originalFontMetrics;
  qreal shiftUp, shiftDown;
public:
  StackedTextBox(const QString &topText,
                 const QString &bottomText,
                 const QPointF &offset, // TODO remove
                 const QFont & font);
  void render(QPainter *painter) const override;
  QRectF boundingRect() const override;
};
} // namespace Molsketch

#endif // MOLSKETCH_STACKEDTEXTBOX_H
