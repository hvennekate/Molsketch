#ifndef MOLSKETCH_TEXTBOX_H
#define MOLSKETCH_TEXTBOX_H

#include <QFont>
#include <QFontMetricsF>
#include <QPointF>


namespace Molsketch {

class TextBox {
protected:
  QPointF offset; // TODO removes
  QFont font;
  QFontMetricsF metrics;
  TextBox(const QPointF &offset, const QFont &font) : offset(offset), font(font), metrics(font) {}
public:
  virtual void render(QPainter *painter) const = 0;
  virtual QRectF boundingRect() const = 0;
  virtual ~TextBox(){}
};


} // namespace Molsketch

#endif // MOLSKETCH_TEXTBOX_H
