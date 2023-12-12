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

#ifndef MOLSKETCH_PAINTABLEAGGREGATE_H
#define MOLSKETCH_PAINTABLEAGGREGATE_H

#include "paintable.h"
#include "debuggable.h"

namespace Molsketch {

  class PaintableAggregate : public Paintable {
    class PaintableAggregatePrivate;
    PaintableAggregatePrivate *d;
    QPointF getCenterShift() const;
    QDebug debug(QDebug debug) const override;
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
