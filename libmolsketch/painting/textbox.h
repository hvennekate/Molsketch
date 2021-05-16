#ifndef MOLSKETCH_TEXTBOX_H
#define MOLSKETCH_TEXTBOX_H

#include <QFont>
#include <QFontMetricsF>
#include <QPointF>


namespace Molsketch {

class TextBox {
protected:
  QPointF offset;
  QFont font;
  QFontMetricsF metrics; // TODO should really be QFontMetricsF
  TextBox(const QPointF &offset, const QFont &font) : offset(offset), font(font), metrics(font) {}
public:
  virtual void render(QPainter *painter) const = 0;
  virtual QRectF boundingRect() const = 0;
};


} // namespace Molsketch

#endif // MOLSKETCH_TEXTBOX_H
