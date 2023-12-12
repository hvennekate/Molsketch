/***************************************************************************
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
 *      --- in part based on code originally written by: ---               *
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
 *   Copyright (C) 2009 by Nicola Zonta                                    *
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
#include "arrow.h"
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QDialog>
#include <QUndoCommand>
#include <QMenu>

#include "scenesettings.h"
#include "settingsitem.h"
#include "molscene.h"
#include "math2d.h"
#include "actions/arrowtypeaction.h"
#include "arrowpopup.h"

namespace Molsketch {


  struct Arrow::privateData : public Arrow::Properties
  {
  };

  Arrow::Arrow(QGraphicsItem *parent)
    : graphicsItem(parent),
      d(new privateData)
  {
    d->arrowType = LowerBackward | UpperBackward ; // TODO rename these
    d->points << QPointF(0,0) << QPointF(50.0, 0.0),
    d->spline = true ;
  }

  Arrow::~Arrow()
  {
    delete d ;
  }

  bool Arrow::splinePossible(const int &numberOfPoints)
  {
    return 1 == numberOfPoints % 3;
  }


  void Arrow::setArrowType(const ArrowType &type)
  {
    d->arrowType = type ;
  }

  void Arrow::setArrowTipPart(const ArrowTypeParts &part)
  {
    d->arrowType |= part;
  }

  void Arrow::unsetArrowTipPart(const ArrowTypeParts &part)
  {
    d->arrowType &= ~part ;
  }

  Arrow::ArrowType Arrow::getArrowType() const
  {
    return d->arrowType;
  }

  void Arrow::setCoordinates(const QVector<QPointF> &c)
  {
    d->points = c ;
  }

  QPolygonF Arrow::coordinates() const
  {
    return d->points ;
  }

  inline QPainterPath generateArrowTip(const QPointF& target,
                                       const QPointF& origin,
                                       const QPointF& translate,
                                       bool up,
                                       bool down,
                                       qreal scaling)
  {
    QPainterPath path ;
    QPointF line(normalized(target - origin) / 10. * scaling) ; // TODO 10 is 5+5 here, the orthogonal vector. Extract!
    QPointF orthogonal(line.y(), -line.x()) ;
    path.moveTo(target) ;
    if (up) path.lineTo(target - 15*line + 5*orthogonal);
    path.lineTo(target - 12*line);
    if (down) path.lineTo(target - 15*line - 5*orthogonal);
    path.lineTo(target);
    path.translate(-translate);
    return path ;
  }

  void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
  {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (d->points.size() < 2) return ;

    // draw the bounding rect if the arrow is selected
    if (isSelected() /* && !m_hoverBegin && !m_hoverEnd */) {
      painter->save();
      painter->setPen(Qt::blue);
      painter->drawRect(boundingRect());
      painter->restore();
    }

    if (d->points.isEmpty()) return ;

    // prepare pen and brush
    painter->save();
    QPen pen;
    pen.setWidthF(lineWidth()) ;
    pen.setCapStyle(Qt::RoundCap) ;
    pen.setJoinStyle(Qt::RoundJoin) ;
    pen.setColor(getColor()) ;
    painter->setPen(pen) ;

    QPainterPath path ;
    // draw the line
    path.moveTo(d->points.first()) ;
    if (d->spline && splinePossible(d->points.size()))
    {
      for (int i = 1 ; i+2 < d->points.size() ; i += 3)
        path.cubicTo(d->points[i],
                     d->points[i+1],
            d->points[i+2]);
      if (isSelected()) // Draw help lines
      {
        painter->save();
        painter->setPen(Qt::gray) ;
        QPointF previous(d->points.first()) ;
        for (int i = 1 ; i+2 < d->points.size() ; i += 3)
        {
          painter->drawLine(previous, d->points[i]) ;
          painter->drawLine(d->points[i+1], d->points[i+2]);
          previous = d->points[i+2] ;
        }
        painter->restore();
      }
    }
    else
      foreach(const QPointF p, d->points.mid(1))
        path.lineTo(p) ;

    path.translate(-pos());
    painter->drawPath(path) ;

    // draw arrow tips
    painter->setBrush(pen.color());
    qreal tipScaling = relativeWidth();
    if (MolScene *sc = qobject_cast<MolScene*>(scene()))
      tipScaling *= sc->settings()->arrowTipWidth()->get();
    if ((UpperBackward | LowerBackward) & d->arrowType)
      painter->drawPath(generateArrowTip(d->points.last(),
                                         d->points[d->points.size()-2],
                        pos(),
                        UpperBackward & d->arrowType,
                        LowerBackward & d->arrowType,
                        tipScaling
                        ));
    if ((UpperForward | LowerForward) & d->arrowType)
      painter->drawPath(generateArrowTip(d->points.first(),
                                         d->points[1],
                        pos(),
                        LowerForward & d->arrowType,
                        UpperForward & d->arrowType,
                        tipScaling
                        )) ;
    painter->restore();

    graphicsItem::paint(painter, option, widget);
  }

  QRectF Arrow::boundingRect() const
  {
    QRectF result ;
    foreach(const QPointF& p, d->points)
      result |= QRectF(p,QSizeF(1,1)) ;
    return result.adjusted(-10,-10,10,10) ;
  }

  void Arrow::setPoint(const int &index, const QPointF &p)
  {
    if (index == d->points.size()) setPos(p) ;
    if (index > d->points.size() || index < 0) return ;
    d->points[index] = p ;
  }

  void Arrow::setPoints(const QPolygonF &polygon)
  {
    d->points = polygon;
  }

  QPointF Arrow::getPoint(const int &index) const
  {
    if (index == d->points.size()) return pos() ;
    if (index > d->points.size() || index < 0) return QPointF() ;
    return d->points[index] ;
  }

  QPointF Arrow::lastPoint() const
  {
    if (d->points.isEmpty()) return QPointF();
    return d->points.last();
  }

  QPointF Arrow::firstPoint() const
  {
    if (d->points.isEmpty()) return QPointF();
    return d->points.first();
  }

  int Arrow::coordinateCount() const
  {
    return d->points.size() + 1 ;
  }

  void Arrow::swapPoint(const int &index, QPointF &p)
  {
    if (index == d->points.size())
    {
      QPointF t = pos() ;
      setPos(p) ;
      p = t ;
      return ;
    }
    if (index >= 0 && index < d->points.size())
      qSwap(d->points[index], p) ;
  }

  QString Arrow::xmlName() const {
    return xmlClassName();
  }

  QString Arrow::xmlClassName() {
    return "arrow";
  }

  Arrow::Properties Arrow::getProperties() const
  {
    return *d;
  }

  QWidget *Arrow::getPropertiesWidget()
  {
    ArrowPopup *widget = new ArrowPopup;
    widget->connectArrow(this);
    return widget;
  }

  void Arrow::setProperties(const Arrow::Properties &p)
  {
    *(dynamic_cast<Arrow::Properties*>(d)) = p;
  }

  void Arrow::readGraphicAttributes(const QXmlStreamAttributes &attributes)
  {
    // Check for legacy arrow type
    auto legacyArrowType = attributes.value("type").toString();
    // if not legacy type, read arrow tip type and return
    if ("ReactionArrow" != legacyArrowType // TODO make these names constants (see MolScene::produceChild())
        && "MechanismArrow" != legacyArrowType)
    {
      d->arrowType = (ArrowType) (attributes.value("arrowType").toString().toInt()) ;
      d->spline = ! (attributes.value("splineDisabled").toString().toInt());
      return;
    }

    // Code for legacy version
    if ("ReactionArrow" == legacyArrowType)
    {
      enum LegacyReactionArrowType {
        SingleArrow = 0,
        DoubleArrow,
        Equilibrium,
        EqRightShifted,
        EqLeftShifted
      };
      // Arrow tip
      auto legacyReactionArrowType = (LegacyReactionArrowType) (attributes.value("arrowType").toString().toInt());
      switch(legacyReactionArrowType)
      {
        case SingleArrow:
          setArrowType(UpperBackward | LowerBackward);
          break;
        case DoubleArrow:
          setArrowType(LowerForward | UpperForward | LowerBackward | UpperBackward);
          break;
        case Equilibrium:
        case EqRightShifted:
        case EqLeftShifted:
          setArrowType(UpperBackward);
          break;
        default:
          setArrowType(NoArrow);
      }

      // Coordinates
      QPointF origin(attributes.value("posx").toString().toDouble(),
                     attributes.value("posy").toString().toDouble());
      QLineF arrowLine(origin, origin +
                       QPointF(attributes.value("endx").toString().toDouble(),
                               attributes.value("endy").toString().toDouble()));
      setCoordinates(QPolygonF() << arrowLine.p1() << arrowLine.p2());

      if (!scene()) return;

      // Fix equilibrium arrows:
      if (Equilibrium == legacyReactionArrowType
          || EqLeftShifted == legacyReactionArrowType
          || EqRightShifted == legacyReactionArrowType)
      { // shift both arrows in equilibrium
        QLineF normalVector = arrowLine.normalVector().unitVector();
        QLineF unitVector = arrowLine.unitVector();
        QPointF normalTranslation = 2*(normalVector.p2() - normalVector.p1());
        QPointF unitTranslation = 15*(unitVector.p2() - unitVector.p1());
        QLineF reverseArrowLine = arrowLine;
        arrowLine.translate(normalTranslation);
        reverseArrowLine.translate(-normalTranslation);
        if (EqRightShifted == legacyReactionArrowType)
        {
          reverseArrowLine.setP1(reverseArrowLine.p1() + unitTranslation);
          reverseArrowLine.setP2(reverseArrowLine.p2() - unitTranslation);
        }
        if (EqLeftShifted == legacyReactionArrowType)
        {
          arrowLine.setP1(arrowLine.p1() + unitTranslation);
          arrowLine.setP2(arrowLine.p2() - unitTranslation);
        }
        auto reverseArrow = new Arrow;
        reverseArrow->setParentItem(parentItem());
        scene()->addItem(reverseArrow);
        reverseArrow->setCoordinates(QPolygonF() << reverseArrowLine.p1()
                                     << reverseArrowLine.p2());
        reverseArrow->setArrowType(LowerForward);
        setCoordinates(QPolygonF() << arrowLine.p1()
                       << arrowLine.p2());
      }
    }
    if ("MechanismArrow" == legacyArrowType)
    {
      enum LegacyMechanismArrowType {
        SingleArrowRight = 0,
        SingleArrowLeft,
        DoubleMechanismArrow,
        SingleHookRight,
        SingleHookLeft,
        DoubleHook
      };
      // Arrow tip
      auto legacyMechanismArrowType = (LegacyMechanismArrowType) (attributes.value("arrowType").toString().toInt());
      switch(legacyMechanismArrowType)
      {
        case SingleArrowRight:
          setArrowType(UpperBackward | LowerBackward);
          break;
        case SingleArrowLeft:
          setArrowType(UpperForward | LowerForward);
          break;
        case DoubleMechanismArrow:
          setArrowType(LowerForward | UpperForward | LowerBackward | UpperBackward);
          break;
        case SingleHookRight:
          setArrowType(UpperBackward);
          break;
        case SingleHookLeft:
          setArrowType(UpperForward);
          break;
        case DoubleHook:
          setArrowType(UpperForward | UpperBackward);
          break;
        default:
          setArrowType(NoArrow);
      }

      // Setting coordinates
      QPolygonF points;
      for (int i = 0 ; i < 4 ; ++i)
        points << QPointF(attributes.value("p" + QString::number(i+1) + "x").toString().toDouble(),
                          attributes.value("p" + QString::number(i+1) + "y").toString().toDouble());
      points.translate(attributes.value("posx").toString().toDouble(),
                       attributes.value("posy").toString().toDouble());
      setCoordinates(points);
    }
  }

  QXmlStreamAttributes Arrow::graphicAttributes() const
  {
    QXmlStreamAttributes attributes ;
    attributes.append("arrowType", QString::number(d->arrowType)) ;
    attributes.append("splineDisabled", QString::number(!d->spline));
    return attributes ;
  }

  void Arrow::prepareContextMenu(QMenu *contextMenu)
  {
    MolScene *sc = qobject_cast<MolScene*>(scene());
    if (sc)
    {
      arrowTypeAction *action = sc->findChild<arrowTypeAction*>();
      if (action)
      {
        contextMenu->addAction(action);
        QObject::connect(action, SIGNAL(triggered()), contextMenu, SLOT(close()));
      }
    }
    graphicsItem::prepareContextMenu(contextMenu);
  }

  qreal Arrow::sceneLineWidth(MolScene *scene) const {
    return scene->settings()->arrowWidth()->get();
  }
  bool Arrow::getSpline() const
  {
    return d->spline;
  }

  void Arrow::setSpline(const bool &value)
  {
    d->spline = value;
  }

} // namespace
