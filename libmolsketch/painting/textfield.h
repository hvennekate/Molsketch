#ifndef MOLSKETCH_TEXTFIELD_H
#define MOLSKETCH_TEXTFIELD_H

#include "paintableaggregate.h"

namespace Molsketch {

  class TextLine;

  class TextField : public PaintableAggregate {
    QRectF addRectFBefore(const QRectF &base, QRectF toAdd) const override;
    QRectF addRectFAfter(const QRectF &base, QRectF toAdd) const override;
    QPointF beforeItemPreShift(const Paintable *item) const override;
    QPointF beforeItemPostShift(const Paintable *item) const override;
    QPointF afterItemPreShift(const Paintable *item) const override;
    QPointF afterItemPostShift(const Paintable *item) const override;
  public:
    explicit TextField(const TextLine *centerLine);
    void addLineAbove(const TextLine *newLine);
    void addLineBelow(const TextLine *newLine);
  };

} // namespace Molsketch

#endif // MOLSKETCH_TEXTFIELD_H
