#ifndef MOLSKETCH_TEXTBOX_H
#define MOLSKETCH_TEXTBOX_H

#include "paintable.h"

#include <QFont>
#include <QFontMetricsF>
#include <QPointF>


namespace Molsketch {

  class TextBox : public Paintable {
  protected:
    QPointF offset; // TODO remove
    QFont font;
    QFontMetricsF metrics;
    TextBox(const QPointF &offset, const QFont &font) : offset(offset), font(font), metrics(font) {}
    // TODO: anchor points
  };


} // namespace Molsketch

#endif // MOLSKETCH_TEXTBOX_H
