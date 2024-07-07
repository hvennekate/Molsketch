/***************************************************************************
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
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
#include "grid.h"
#include "scenesettings.h"
#include "settingsitem.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QtMath>
#include <QDebug>

namespace Molsketch {

  // TODO unit test this class (especially the ::alignPoint() function)

  struct Grid::privateData {
    SceneSettings *settings;
  };

  Grid::Grid(SceneSettings *settings)
    : d(new privateData) {
    setZValue(-INFINITY);
    setFlags(GraphicsItemFlags());
    d->settings = settings;
  }

  Grid::~Grid() {
    delete d;
  }

  qreal getNearest(qreal value, qreal step) {
    return qRound(value/step) * step;
  }

  QPointF getNearest(const QPointF& original, qreal xStep, qreal yStep) {
    return QPointF(getNearest(original.x(), xStep),
                   getNearest(original.y(), yStep));
  }

  QPointF Grid::alignPoint(const QPointF &point) {
    return getNearest(point, d->settings->horizontalGridSpacing()->get(),
                      d->settings->verticalGridSpacing()->get());
  }

  void Grid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
  {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    QPen pen(d->settings->gridColor()->get());
    pen.setWidth(d->settings->gridLineWidth()->get());
    painter->setPen(pen);

    QRectF rect(boundingRect().normalized());
    qreal horizontalStep = d->settings->horizontalGridSpacing()->get();
    qreal verticalStep = d->settings->verticalGridSpacing()->get();
    for (qreal x = getNearest(rect.left(), horizontalStep) ; x < rect.right() ; x = getNearest(x + horizontalStep, horizontalStep))
      painter->drawLine(x, rect.top(), x, rect.bottom());
    for (qreal y = getNearest(rect.top(), verticalStep) ; y < rect.bottom() ; y = getNearest(y + verticalStep, verticalStep))
        painter->drawLine(rect.left(), y, rect.right(), y);
    painter->restore();
  }

  QRectF Grid::boundingRect() const {
    if (!scene()) return QRectF();
    return scene()->sceneRect();
  }

} // namespace Molsketch

