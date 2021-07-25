#ifndef PAINTABLE_H
#define PAINTABLE_H

#include "boundingboxlinker.h"

class QPainter;

namespace Molsketch {

  class Paintable : public Linkable {
  public:
    virtual ~Paintable() {}
    virtual void paint(QPainter *painter) const = 0;
    virtual QRectF boundingRect() const = 0;
    QPointF getAnchorPoint(const Anchor &anchor) const override;
  };

} // namespace Molsketch

#endif // PAINTABLE_H
