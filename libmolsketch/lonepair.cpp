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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "graphicsitem.h"
#include "lonepair.h"
#include <QDebug>
#include <QPen>

namespace Molsketch {

class LonePairPrivate {
public:
  BoundingBoxLinker linker;
};

LonePair::LonePair (qreal angle, qreal lineWidth, qreal length, BoundingBoxLinker linker, const QColor& color)
  : QGraphicsLineItem(QLineF::fromPolar(length, angle)), // TODO scale line with font size (or introduce other object for that)
    d_ptr(new LonePairPrivate)
{
  Q_D(LonePair);
  d->linker = linker;
  QPen pen(color, lineWidth);
  pen.setCapStyle(Qt::RoundCap);
  setPen(pen);
}

LonePair::LonePair (const LonePair& other)
  : QGraphicsLineItem(other.line()),
    d_ptr(new LonePairPrivate(*(other.d_ptr)))
{
  setPen(other.pen());
}

LonePair::~LonePair () {}

BoundingBoxLinker LonePair::linker () const {
  Q_D(const LonePair);
  return d->linker;
}

bool LonePair::operator ==(const LonePair &other) {
  return other.d_ptr->linker == d_ptr->linker
      && other.line() == line()
      && other.pen() == pen();
}

#ifdef QT_DEBUG
QDebug operator<< (QDebug debug, const LonePair& lonePair) {
  return debug.nospace() << "LonePair("
                         << "line: " << lonePair.line()
                         << ", pen: " << lonePair.pen()
                         << ", linker: " << lonePair.d_ptr->linker
                         << ")";
}
#endif

QRectF LonePair::boundingRect () const {
  if (!parentItem()) return QRectF();
  Q_D(const LonePair);
  QRectF originalBounds = QGraphicsLineItem::boundingRect();
  QPointF shift = d->linker.getShift(parentItem()->boundingRect(), originalBounds);
  return originalBounds.translated(shift);
}

void LonePair::paint (QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  if (!parentItem()) return;
  Q_D(LonePair);
  QRectF originalBounds = QGraphicsLineItem::boundingRect(); // TODO join with code from boundingRect()
  QPointF shift = d->linker.getShift(parentItem()->boundingRect(), originalBounds);
  setLine(line().translated(shift));
  QGraphicsLineItem::paint(painter, option, widget);
}

QString LonePair::xmlName () const {
  return xmlClassName();
}

QString LonePair::xmlClassName() {
  return "lonePair";
}

qreal LonePair::angle() const {
  return line().angle();
}

qreal LonePair::length() const {
  return line().length(); // TODO use relative dimension
}

qreal LonePair::lineWidth() const {
  return pen().widthF(); // TODO use relative dimension
}

XmlObjectInterface* LonePair::produceChild (const QString& name, const QXmlStreamAttributes& attributes) {
  Q_D(LonePair);
  if (d->linker.xmlName() == name) return &(d->linker);
  return abstractXmlObject::produceChild(name, attributes);
}

void LonePair::readAttributes (const QXmlStreamAttributes& attributes) {
  QPen newPen = pen();
  newPen.setWidthF(attributes.value("lineWidth").toDouble());
  newPen.setColor(graphicsItem::extractColor(attributes));
  setPen(newPen);
  setLine(QLineF::fromPolar(attributes.value("length").toDouble(),
                            attributes.value("angle").toDouble()));
}

QList<const XmlObjectInterface*> LonePair::children () const {
  Q_D(const LonePair);
  return QList<const XmlObjectInterface*>() << &(d->linker);
}

QXmlStreamAttributes LonePair::xmlAttributes () const {
  QXmlStreamAttributes attributes;
  attributes.append("angle", QString::number(line().angle()));
  attributes.append("length", QString::number(line().length()));
  attributes.append("lineWidth", QString::number(pen().widthF()));
  graphicsItem::addColor(attributes, pen().color());
  return attributes;
}

} // namespace Molsketch
