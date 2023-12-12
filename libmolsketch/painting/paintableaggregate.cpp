/***************************************************************************
 *   Copyright (C) 2022 by Hendrik Vennekate                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "paintableaggregate.h"
#include <QDebug>
#include <QDebug>
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

  QDebug PaintableAggregate::debug(QDebug debug) const {
    debug << typeid(*this).name() << "[[ ";
    for (const Debuggable *deb : d->before)
      debug << deb << " ";
    debug << "] " << d->center.data() << " [ ";
    for (const Debuggable *deb : d->after)
      debug << deb << " ";
    return debug << "]]";
  }

  void PaintableAggregate::addBefore(const Paintable *p) {
    if (p) d->before << p;
  }

  void PaintableAggregate::addAfter(const Paintable *p) {
    if (p) d->after << p;
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
    auto centerBounds = d->center->boundingRect();
    auto resultBox = std::transform_reduce(d->before.cbegin(), d->before.cend(),
                                           centerBounds,
                                           // TODO Method reference?
                                           [&](const QRectF &base, QRectF toAdd) { return addRectFBefore(base, toAdd); },
                                           boundingRectFromPointer);
    resultBox = std::transform_reduce(d->after.cbegin(), d->after.cend(),
                                      resultBox,
                                      [&](const QRectF &base, QRectF toAdd) { return addRectFAfter(base, toAdd); },
                                      boundingRectFromPointer);
    return resultBox.translated(- d->center->getAnchorPoint(Anchor::Center));
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
