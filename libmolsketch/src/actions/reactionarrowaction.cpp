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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "reactionarrowaction.h"
#include "arrow.h"
#include "molscene.h"
#include "commands.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

namespace Molsketch {

  class reactionArrowAction::privateData
  {
  public:
    privateData(QObject* parent)
#define ARROWACTION(NAME,DESCRIPTION,ICON) NAME(new QAction(QIcon(":images/" ICON ".png"), tr(DESCRIPTION), parent))
      : ARROWACTION(normalArrow, "Single arrow", "simplearrow"),
        ARROWACTION(doubleArrow, "Double arrow", "doublearrow"),
        ARROWACTION(hookArrow, "Half arrow", "halfarrow"),
        currentArrow(0)
    {}
    ~privateData()
    {
    }
    QAction *normalArrow,
    *doubleArrow,
    *hookArrow ;
    Arrow* currentArrow ;
    QPointF mousePressPosition;
  };

  reactionArrowAction::reactionArrowAction(MolScene *scene)
    : multiAction(scene),
      d(new privateData(this))
  {
    setText(tr("Arrow"));
    addSubAction(d->normalArrow) ;
    addSubAction(d->doubleArrow) ;
    addSubAction(d->hookArrow) ;
  }

  reactionArrowAction::~reactionArrowAction()
  {
    delete d ;
  }

  void reactionArrowAction::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    if (event->button() != Qt::LeftButton) return ;
    if (event->modifiers() != Qt::NoModifier) return ;
    if (d->currentArrow) delete d->currentArrow ;
    d->currentArrow = new Arrow() ;
    if (activeSubAction() == d->doubleArrow)
      d->currentArrow->setArrowType(Arrow::LowerForward
                                    | Arrow::UpperForward
                                    | Arrow::LowerBackward
                                    | Arrow::UpperBackward);
    if (activeSubAction() == d->hookArrow)
      d->currentArrow->setArrowType(Arrow::UpperBackward);
    d->mousePressPosition = event->scenePos();
    d->currentArrow->setCoordinates(QVector<QPointF>(2, d->mousePressPosition)) ;
    scene()->addItem(d->currentArrow) ;
    scene()->update(d->currentArrow->boundingRect());
    event->accept();
  }

  void reactionArrowAction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  {
    if (!d->currentArrow) return ;
    d->currentArrow->setPoints(
          makePolygon(QLineF(
                        d->mousePressPosition, // TODO event->buttonDownScenePos
                        event->scenePos())));
    scene()->update(d->currentArrow->boundingRect());
    event->accept();
  }

  void reactionArrowAction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    Q_UNUSED(event)
    if (!d->currentArrow) return ;
    if (!scene() || !scene()->stack()) return ; // TODO
    scene()->stack()->push(new Commands::AddItem(d->currentArrow, scene())) ;
    d->currentArrow = 0 ;
    event->accept();
  }

  QPolygonF reactionArrowAction::makePolygon(const QLineF &line)
  {
    return QPolygonF() << line.p1() << line.p2() ;
  }

} // namespace
