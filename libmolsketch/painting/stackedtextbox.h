#ifndef MOLSKETCH_STACKEDTEXTBOX_H
#define MOLSKETCH_STACKEDTEXTBOX_H

#include "textbox.h"

namespace Molsketch {

class StackedTextBox : public TextBox {
  QString topText, bottomText;
  int shiftUp, shiftDown, mainHeight;
public:
  StackedTextBox(const QString &topText,
                 const QString &bottomText,
                 const QPointF &offset,
                 const QFont & font,
                 const QFontMetricsF & mainFontMetrics);
  void render(QPainter *painter) const override;
  QRectF boundingRect() const override;
};
} // namespace Molsketch

#endif // MOLSKETCH_STACKEDTEXTBOX_H
