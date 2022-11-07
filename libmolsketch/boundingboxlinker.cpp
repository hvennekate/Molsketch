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
#include "boundingboxlinker.h"

#include <QDebug>
#include <QRegularExpression>
#include <QString>

namespace Molsketch {

  struct BoundingBoxLinkerPrivate {
    Anchor origin;
    Anchor target;
    QPointF offset;
    bool operator ==(const BoundingBoxLinkerPrivate& other) const {
      return origin == other.origin && target == other.target && offset == other.offset;
    }
  };

  BoundingBoxLinker::BoundingBoxLinker(Anchor originAnchor, Anchor ownAnchor, const QPointF &offset)
    : d_ptr(new BoundingBoxLinkerPrivate) {
    Q_D(BoundingBoxLinker);
    d->origin = originAnchor;
    d->target = ownAnchor;
    d->offset = offset;
  }

  BoundingBoxLinker::BoundingBoxLinker(const BoundingBoxLinker &other)
    :d_ptr(new BoundingBoxLinkerPrivate(*(other.d_ptr)))
  {}

  BoundingBoxLinker::~BoundingBoxLinker() {}

  class RectFLinkable : public Linkable {
    QRectF rect;
  public:
    RectFLinkable(const QRectF& rect) : rect(rect) {}
    QPointF getAnchorPoint(const Anchor &anchor) const override {
      return Linkable::getAnchorPoint(rect, anchor);
    }
  };

  QPointF BoundingBoxLinker::getShift(const QRectF &reference, const QRectF &target) const {
    return getShift(RectFLinkable(reference), RectFLinkable(target));
  }

  QPointF BoundingBoxLinker::getShift(const Linkable &reference, const Linkable &target) const {
    Q_D(const BoundingBoxLinker);
    return reference.getAnchorPoint(d->origin)
        - target.getAnchorPoint(d->target)
        + d->offset;
  }

  QString BoundingBoxLinker::xmlName() const {
    return xmlClassName();
  }

  QString BoundingBoxLinker::xmlClassName() {
    return "bbLinker";
  }

  QXmlStreamAttributes BoundingBoxLinker::xmlAttributes() const {
    Q_D(const BoundingBoxLinker);
    QXmlStreamAttributes attributes;
    attributes.append("originAnchor", toString(d->origin));
    attributes.append("targetAnchor", toString(d->target));
    attributes.append("xOffset", QString::number(d->offset.x()));
    attributes.append("yOffset", QString::number(d->offset.y()));
    return attributes;
  }

  void BoundingBoxLinker::readAttributes(const QXmlStreamAttributes &attributes) {
    Q_D(BoundingBoxLinker);
    d->origin = anchorFromString(attributes.value("originAnchor").toString());
    d->target = anchorFromString(attributes.value("targetAnchor").toString());
    d->offset.setX(attributes.value("xOffset").toDouble());
    d->offset.setY(attributes.value("yOffset").toDouble());
  }

  bool BoundingBoxLinker::operator==(const BoundingBoxLinker &other) const {
    Q_D(const BoundingBoxLinker);
    const BoundingBoxLinkerPrivate* const otherD = other.d_func();
    return *d == *otherD;
  }

  BoundingBoxLinker &BoundingBoxLinker::operator=(const BoundingBoxLinker &other) {
    if (&other != this) d_ptr.reset(new BoundingBoxLinkerPrivate(*(other.d_ptr)));
    return *this;
  }

  int convertAnchor(const Anchor &anchor) {
    return static_cast<char>(anchor);
  }

  Anchor convertAnchor(const int &integer) {
    Anchor result = static_cast<Anchor>(integer);
    switch(result) {
      case Anchor::Top:
      case Anchor::Bottom:
      case Anchor::Left:
      case Anchor::Right:
      case Anchor::TopLeft:
      case Anchor::BottomLeft:
      case Anchor::TopRight:
      case Anchor::BottomRight:
      case Anchor::Center:
        return result;
      default:
        return Anchor::Center;
    }
  }

  Anchor anchorFromString(QString input) {
    auto cleaned = input.remove(QRegularExpression("\\s")).toLower();
    if ("center" == cleaned) return Anchor::Center;
    if ("left" == cleaned) return Anchor::Left;
    if ("right" == cleaned) return Anchor::Right;
    if ("top" == cleaned) return Anchor::Top;
    if ("bottom" == cleaned) return Anchor::Bottom;
    if ("topleft" == cleaned) return Anchor::TopLeft;
    if ("topright" == cleaned) return Anchor::TopRight;
    if ("bottomleft" == cleaned) return Anchor::BottomLeft;
    if ("bottomright" == cleaned) return Anchor::BottomRight;
    return Anchor::Center;
  }

  QString toString(const Anchor &anchor) {
    switch(anchor) {
      case Anchor::Center: return "Center";
      case Anchor::Left: return "Left";
      case Anchor::Right: return "Right";
      case Anchor::Top: return "Top";
      case Anchor::Bottom: return "Bottom";
      case Anchor::TopLeft: return "TopLeft";
      case Anchor::TopRight: return "TopRight";
      case Anchor::BottomLeft: return "BottomLeft";
      case Anchor::BottomRight: return "BottomRight";
    }
    return "Center";
  }

#ifdef QT_DEBUG
  QDebug operator<<(QDebug debug, const Anchor &anchor) {
    debug << toString(anchor);
    return debug;
  }

  QDebug operator<<(QDebug debug, const BoundingBoxLinker &linker) {
    debug.nospace() << "BoundingBoxLinker(origin ref: " << linker.d_func()->origin
                    << ", target ref: " << linker.d_func()->target
                    << ", offset: " << linker.d_func()->offset
                    << ")";
    return debug;
  }
#endif

  BoundingBoxLinker BoundingBoxLinker::above() { return BoundingBoxLinker(Anchor::Top, Anchor::Bottom); }
  BoundingBoxLinker BoundingBoxLinker::below() { return BoundingBoxLinker(Anchor::Bottom, Anchor::Top); }
  BoundingBoxLinker BoundingBoxLinker::toLeft() { return BoundingBoxLinker(Anchor::Left, Anchor::Right); }
  BoundingBoxLinker BoundingBoxLinker::toRight() { return BoundingBoxLinker(Anchor::Right, Anchor::Left); }
  BoundingBoxLinker BoundingBoxLinker::upperLeft() { return BoundingBoxLinker(Anchor::TopLeft, Anchor::BottomRight); }
  BoundingBoxLinker BoundingBoxLinker::upperRight() { return BoundingBoxLinker(Anchor::TopRight, Anchor::BottomLeft); }
  BoundingBoxLinker BoundingBoxLinker::lowerLeft() { return BoundingBoxLinker(Anchor::BottomLeft, Anchor::TopRight); }
  BoundingBoxLinker BoundingBoxLinker::lowerRight() { return BoundingBoxLinker(Anchor::BottomRight, Anchor::TopLeft); }

  // TODO consider converting constructor
  BoundingBoxLinker BoundingBoxLinker::atBottom() { return BoundingBoxLinker(Anchor::Bottom); }
  BoundingBoxLinker BoundingBoxLinker::atTop() { return BoundingBoxLinker(Anchor::Top); }
  BoundingBoxLinker BoundingBoxLinker::atLeft() { return BoundingBoxLinker(Anchor::Left); }
  BoundingBoxLinker BoundingBoxLinker::atRight() { return BoundingBoxLinker(Anchor::Right); }
  BoundingBoxLinker BoundingBoxLinker::atTopLeft() { return BoundingBoxLinker(Anchor::TopLeft); }
  BoundingBoxLinker BoundingBoxLinker::atTopRight() { return BoundingBoxLinker(Anchor::TopRight); }
  BoundingBoxLinker BoundingBoxLinker::atBottomLeft() { return BoundingBoxLinker(Anchor::BottomLeft); }
  BoundingBoxLinker BoundingBoxLinker::atBottomRight() { return BoundingBoxLinker(Anchor::BottomRight); }

  QPointF Linkable::getAnchorPoint(const QRectF &rect, const Anchor &anchor) {
    switch (anchor) {
      case Anchor::BottomLeft: return rect.bottomLeft();
      case Anchor::BottomRight: return rect.bottomRight();
      case Anchor::TopLeft: return rect.topLeft();
      case Anchor::TopRight: return rect.topRight();
      case Anchor::Bottom: return (rect.bottomLeft() + rect.bottomRight())/2.;
      case Anchor::Left: return (rect.topLeft() + rect.bottomLeft())/2.;
      case Anchor::Right: return (rect.topRight() + rect.bottomRight())/2.;
      case Anchor::Top: return (rect.topLeft() + rect.topRight())/2.;
      case Anchor::Center:
      default: return rect.center();
    }
  }

} // namespace Molsketch
