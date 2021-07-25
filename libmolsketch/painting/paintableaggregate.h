#ifndef MOLSKETCH_PAINTABLEAGGREGATE_H
#define MOLSKETCH_PAINTABLEAGGREGATE_H

#include "paintable.h"

namespace Molsketch {

  class PaintableAggregate : public Paintable {
    class PaintableAggregatePrivate;
    PaintableAggregatePrivate *d;
    QPointF getCenterShift() const;
  protected:
    void addBefore(const Paintable *p);
    void addAfter(const Paintable *p);
    virtual QRectF addRectFBefore(const QRectF &base, QRectF toAdd) const = 0;
    virtual QRectF addRectFAfter(const QRectF &base, QRectF toAdd) const = 0;
    virtual QPointF beforeItemPreShift(const Paintable *item) const = 0;
    virtual QPointF beforeItemPostShift(const Paintable *item) const = 0;
    virtual QPointF afterItemPreShift(const Paintable *item) const = 0;
    virtual QPointF afterItemPostShift(const Paintable *item) const = 0;
  public:
    explicit PaintableAggregate(const Paintable *center);
    virtual ~PaintableAggregate();
    void paint(QPainter *painter) const override;
    QRectF boundingRect() const override;
    QPointF getAnchorPoint(const Anchor &anchor) const override;
  };

} // namespace Molsketch

#endif // MOLSKETCH_PAINTABLEAGGREGATE_H
