#ifndef MOLSKETCH_TEXTLINE_H
#define MOLSKETCH_TEXTLINE_H

#include "paintableaggregate.h"

namespace Molsketch {

  class TextBox;

  class TextLine : public PaintableAggregate {
    QRectF addRectFBefore(const QRectF &base, QRectF toAdd) const override;
    QRectF addRectFAfter(const QRectF &base, QRectF toAdd) const override;
    QPointF beforeItemPreShift(const Paintable *item) const override;
    QPointF beforeItemPostShift(const Paintable *item) const override;
    QPointF afterItemPreShift(const Paintable *item) const override;
    QPointF afterItemPostShift(const Paintable *item) const override;
  public:
    explicit TextLine(const TextBox *centerBox);
    void addBoxRight(const TextBox *newBox);
    void addBoxLeft(const TextBox *newBox);
  };

} // namespace Molsketch

#endif // MOLSKETCH_TEXTLINE_H
