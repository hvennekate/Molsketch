/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate                               *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "boundingrectassertion.h"

#include <QRectF>
#include <QTextStream>
#include <QVariant>
#include <QIODevice>
#include <QDebug>

class BoundingRectAssertionPrivate {
public:
  QRectF actualRect;
  QVariant actualValue;
  QString message;
  void compareWith(const QRectF& expectedRect, const QVariant& expectedValue) {
    if (actualValue == expectedValue) return;
    QTextStream out(stdout, QIODevice::WriteOnly);
    out << message << Qt::endl;
    QString comparisonMessage;
    QDebug(&comparisonMessage) << "Expected:" << expectedValue << "Actual:" << actualValue << Qt::endl;
    out << comparisonMessage;
    out << "Expected bounds: " ;
    printRect(expectedRect, out);
    out << Qt::endl << "Actual bounds: ";
    printRect(actualRect, out);
    out << Qt::endl;
    throw message;
  }

  void printRect(const QRectF& rect, QTextStream& out) {
    QString rectString;
    QDebug(&rectString) << rect.topLeft() << " -- " << rect.bottomRight();
    out << rectString;
  }
};

BoundingRectAssertion::~BoundingRectAssertion() {}

BoundingRectAssertion::BoundingRectAssertion(const QRectF& rect, const QVariant& value, const QString& message)
  : d_ptr(new BoundingRectAssertionPrivate)
{
  Q_D(BoundingRectAssertion);
  d->actualRect = rect;
  d->actualValue = value;
  d->message = message;
}

BoundingRectAssertion::BoundingRectAssertion(BoundingRectAssertion &&other)
  : d_ptr(other.d_ptr.take()){}

BoundingRectScalarAssertion BoundingRectScalarAssertion::leftEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion(item->boundingRect(), item->boundingRect().left(),
                                     "Left edge mismatch");
}

BoundingRectScalarAssertion BoundingRectScalarAssertion::rightEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion(item->boundingRect(), item->boundingRect().right(),
                                     "Right edge mismatch");
}

BoundingRectScalarAssertion BoundingRectScalarAssertion::bottomEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion(item->boundingRect(), item->boundingRect().bottom(),
                                     "Bottom edge mismatch");
}

BoundingRectScalarAssertion BoundingRectScalarAssertion::topEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion(item->boundingRect(), item->boundingRect().top(),
                                     "Top edge mismatch");
}

BoundingRectScalarAssertion BoundingRectScalarAssertion::centerXCoordinate(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion(item->boundingRect(), item->boundingRect().center().x(),
                                     "Horizontal center mismatch");
}

BoundingRectScalarAssertion BoundingRectScalarAssertion::centerYCoordinate(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion(item->boundingRect(), item->boundingRect().center().y(),
                                     "Vertical center mismatch");
}

BoundingRectScalarAssertion& BoundingRectScalarAssertion::is(qreal distance) {
  d_ptr->actualValue = d_ptr.data()->actualValue.toReal() - distance;
  return *this;
}

BoundingRectScalarAssertion& BoundingRectScalarAssertion::fromLeftEdgeOf(const QGraphicsItem *other) {
  d_ptr->compareWith(other->boundingRect(), other->boundingRect().left());
  return *this;
}

BoundingRectScalarAssertion &BoundingRectScalarAssertion::fromRightEdgeOf(const QGraphicsItem *other) {
  d_ptr->compareWith(other->boundingRect(), other->boundingRect().right());
  return *this;
}

BoundingRectScalarAssertion &BoundingRectScalarAssertion::fromBottomEdgeOf(const QGraphicsItem *other) {
  d_ptr->compareWith(other->boundingRect(), other->boundingRect().bottom());
  return *this;
}

BoundingRectScalarAssertion &BoundingRectScalarAssertion::fromTopEdgeOf(const QGraphicsItem *other) {
  d_ptr->compareWith(other->boundingRect(), other->boundingRect().top());
  return *this;
}

BoundingRectScalarAssertion &BoundingRectScalarAssertion::fromCenterXCoordinateOf(const QGraphicsItem * other){
  d_ptr->compareWith(other->boundingRect(), other->boundingRect().center().x());
  return *this;
}

BoundingRectScalarAssertion &BoundingRectScalarAssertion::fromCenterYCoordinateOf(const QGraphicsItem *other) {
  d_ptr->compareWith(other->boundingRect(), other->boundingRect().center().y());
  return *this;
}

BoundingRectScalarAssertion::BoundingRectScalarAssertion(const QRectF &rect, qreal value, const QString &message)
  : BoundingRectAssertion(rect, value, message) {}

BoundingRectScalarAssertion BoundingRectAssert::rightEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion::rightEdgeOf(item);
}

BoundingRectScalarAssertion BoundingRectAssert::leftEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion::leftEdgeOf(item);
}

BoundingRectScalarAssertion BoundingRectAssert::centerXCoordinateOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion::centerXCoordinate(item);
}

BoundingRectScalarAssertion BoundingRectAssert::centerYCoordinateOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion::centerYCoordinate(item);
}

BoundingRectScalarAssertion BoundingRectAssert::topEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion::topEdgeOf(item);
}

BoundingRectScalarAssertion BoundingRectAssert::bottomEdgeOf(const QGraphicsItem *item) {
  return BoundingRectScalarAssertion::bottomEdgeOf(item);
}
