#ifndef MOLSKETCH_TEXTBOX_H
#define MOLSKETCH_TEXTBOX_H

#include "paintable.h"

#include <QFont>
#include <QFontMetricsF>
#include <QPointF>


namespace Molsketch {

  class TextBox : public Paintable {
  public:
    virtual bool preferredCenter() const = 0;
  protected:
    QFont font;
    QFontMetricsF metrics;
    TextBox(const QFont &font) : font(font), metrics(font) {}
    // TODO: anchor points
  };


} // namespace Molsketch

#endif // MOLSKETCH_TEXTBOX_H
