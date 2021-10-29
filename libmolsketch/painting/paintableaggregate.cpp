#include "paintableaggregate.h"
#include <QPainter>
#include <algorithm>

namespace Molsketch {

  class PaintableAggregate::PaintableAggregatePrivate {
  public:
    QList<const Paintable*> before, after;
    QScopedPointer<const Paintable> center;
    explicit PaintableAggregatePrivate(const Paintable *center)
      : center(center) {}
    ~PaintableAggregatePrivate() {
      auto beforeAndAfterItems = before + after;
      for (auto paintable : qAsConst(beforeAndAfterItems)) delete paintable;
    }
  };

  QPointF PaintableAggregate::getCenterShift() const {
    return -d->center->getAnchorPoint(Anchor::Center);
  }

  void PaintableAggregate::addBefore(const Paintable *p) {
    d->before << p;
  }

  void PaintableAggregate::addAfter(const Paintable *p) {
    d->after << p;
  }

  PaintableAggregate::PaintableAggregate(const Paintable *center)
    : d(new PaintableAggregatePrivate(center)) {}

  PaintableAggregate::~PaintableAggregate() {
    delete d;
  }

  void PaintableAggregate::paint(QPainter *painter) const {
    if (!d->center) return;
    auto initialShift = getCenterShift();

    painter->save();
    painter->translate(initialShift);
    d->center->paint(painter);
    painter->translate(afterItemPostShift(d->center.data()));

    for (auto paintable : qAsConst(d->after)) {
      painter->translate(afterItemPreShift(paintable));
      paintable->paint(painter);
      painter->translate(afterItemPostShift(paintable));
    }
    painter->restore();

    painter->save();
    painter->translate(initialShift);
    painter->translate(beforeItemPostShift(d->center.data()));
    for (auto paintable : qAsConst(d->before)) {
      painter->translate(beforeItemPreShift(paintable));
      paintable->paint(painter);
      painter->translate(beforeItemPostShift(paintable));
    }
    painter->restore();
  }

  QRectF boundingRectFromPointer(const Paintable *p) {
    return p->boundingRect();
  }

  QRectF PaintableAggregate::boundingRect() const {
    if (!d->center) return QRectF();
    auto resultBox = std::transform_reduce(d->before.cbegin(), d->before.cend(),
                                           d->center->boundingRect(),
                                           // TODO Method reference?
                                           [&](const QRectF &base, QRectF toAdd) { return addRectFBefore(base, toAdd); },
                                           boundingRectFromPointer);
    resultBox = std::transform_reduce(d->after.cbegin(), d->after.cend(),
                                      resultBox,
                                      [&](const QRectF &base, QRectF toAdd) { return addRectFAfter(base, toAdd); },
                                      boundingRectFromPointer);
    return resultBox.translated(- d->center->boundingRect().center());
  }

  QPointF PaintableAggregate::getAnchorPoint(const Anchor &anchor) const
  {
    if (!d->center) return QPointF();
    auto subAnchor = d->center->getAnchorPoint(anchor) + getCenterShift();
    switch (anchor) {
      case Anchor::Bottom: return QPointF(subAnchor.x(), boundingRect().bottom());
      case Anchor::Top: return QPointF(subAnchor.x(), boundingRect().top());
      case Anchor::Left: return QPointF(boundingRect().left(), subAnchor.y());
      case Anchor::Right: return QPointF(boundingRect().right(), subAnchor.y());
      case Anchor::TopLeft: return boundingRect().topLeft();
      case Anchor::TopRight: return boundingRect().topRight();
      case Anchor::BottomLeft: return boundingRect().bottomLeft();
      case Anchor::BottomRight: return boundingRect().bottomRight();
      default:
      case Anchor::Center: return subAnchor;
    }
  }

} // namespace Molsketch
