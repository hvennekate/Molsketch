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
                 const QFont & font);
  void paint(QPainter *painter) const override;
  QRectF boundingRect() const override;
  bool preferredCenter() const override;
  QString getTopText() const;
  QString getBottomText() const;
};
} // namespace Molsketch

#endif // MOLSKETCH_STACKEDTEXTBOX_H
