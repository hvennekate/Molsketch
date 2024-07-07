/***************************************************************************
 *   Copyright (C) 2016 Hendrik Vennekate                                  *
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

#include <QGraphicsSceneMouseEvent>
#include "frameaction.h"

#include "molscene.h"
#include "commands.h"
#include "frame.h"
#include "iconutils.h"

namespace Molsketch {

#define FRAMEMACRO(ARROWSTRING, ICON, DESCRIPTION) { auto a = new QAction(getInternalIcon(ICON), tr(DESCRIPTION), this); a->setData(ARROWSTRING); addSubAction(a); }

  // TODO this leads to ugly doubling with FrameTypeWidget
  struct FrameAction::privateData
  {
    Frame *currentFrame;
    QPointF mousePressPosition;
    privateData()
      : currentFrame(0)
    {}
  };

  QString FrameAction::rightBracketFrame()
  {
    return "(-10r.5l1,r.5l1)-+( 10,0)-(r .5l1,r-.5l-1)-+(-10,0)";
  }

  QString FrameAction::leftBracketFrame()
  {
    return "(r-.5l-1,r.5l1)+( 10,0)-+(-10,0)-(r-.5l-1,r-.5l-1)-+( 10,0)";
  }

  QString FrameAction::bracketsFrame()
  {
    return leftBracketFrame() + rightBracketFrame();
  }

  QString FrameAction::angleFrame()
  {
    return "(-20r.5l1,r-.5l-1)-+(20,0)-+(0,20)";
  }

  QString FrameAction::curlyBracketsFrame()
  {
    return leftCurlyBracketFrame() + rightCurlyBracketFrame();
  }

  QString FrameAction::rectangleFrame()
  {
    return "(r.5l1,r.5l1)-(r.5l1,r-.5l-1)-(r-.5l-1,r-.5l-1)-(r-.5l-1,r.5l1)-(r.5l1,r.5l1)";
  }

  QString FrameAction::roundedCornerRectangleFrame()
  {
    return "(-2.5r-.5l-.5,-7.5r.5l0.5).+(0,10).+(10,0)"
           "-(-7.5r.5l0.5,2.5r.5l0.5).+(10,0).+(0,-10)"
           "-(2.5r.5l0.5,7.5r-.5l-.5).+(0,-10).+(-10,0)"
           "-(7.5r-.5l-.5,-2.5r-.5l-.5).+(-10,0).+(0,10)"
           "-(-2.5r-.5l-.5,-7.5r.5l0.5)";
  }

  QString FrameAction::leftCurlyBracketFrame()
  {
    return "(7.5r-.5l-.5,-2.5r-.5l-.5).+(-10,0).+(0,10)"
           "-(-2.5r-.5l-.5,-10).+(0,10).+(-5,0).+(5,0).+(0,10)"
           "-(-2.5r-.5l-.5,-7.5r.5l0.5).+(0,10).+(10,0)";
  }

  QString FrameAction::rightCurlyBracketFrame()
  {
    return "(-7.5r.5l.5,-2.5r-.5l-.5).+(10,0).+(0,10)"
           "-(2.5r.5l0.5,-10).+(0,10).+(5,0).+(-5,0).+(0,10)"
           "-(2.5r.5l0.5,-7.5r.5l0.5).+(0,10).+(-10,0)";
  }

  FrameAction::FrameAction(MolScene *scene)
    : multiAction(scene),
      d(new privateData)
  {
    setText(tr("Decoration"));
    FRAMEMACRO(bracketsFrame(), "bracket", "brackets");
    FRAMEMACRO(leftBracketFrame(), "leftbracket", "left bracket");
    FRAMEMACRO(rightBracketFrame(), "rightbracket", "right bracket");
    FRAMEMACRO(angleFrame(),"angle", "corner");
    FRAMEMACRO(curlyBracketsFrame(), "curlybracket", "curly brackets");
    FRAMEMACRO(leftCurlyBracketFrame(), "leftcurlybracket", "left curly bracket");
    FRAMEMACRO(rightCurlyBracketFrame(), "rightcurlybracket", "right curly bracket");
    FRAMEMACRO(rectangleFrame(), "fullframe", "frame");
    FRAMEMACRO(roundedCornerRectangleFrame(), "roundedfullframe", "frame with rounded corners");
  }

  FrameAction::~FrameAction()
  {
    delete d;
  }

  void FrameAction::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {// TODO refactor with other versions (reactionarrowaction etc)
    if (event->button() != Qt::LeftButton) return ;
    if (event->modifiers() != Qt::NoModifier) return ;
    if (!activeSubAction()) return;
    event->accept();
    if (d->currentFrame) delete d->currentFrame;
    d->currentFrame= new Frame() ;
    d->currentFrame->setFrameString(activeSubAction()->data().toString());
    d->mousePressPosition = event->scenePos();
    d->currentFrame->setCoordinates(QVector<QPointF>(2, d->mousePressPosition)) ;
    scene()->addItem(d->currentFrame) ;
    scene()->update(d->currentFrame->boundingRect());
  }

  void FrameAction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  {
    if (!d->currentFrame) return ;
    event->accept();
    d->currentFrame->setCoordinates( QPolygonF() << d->mousePressPosition
                                     << event->scenePos());
    scene()->update(d->currentFrame->boundingRect());
  }

  void FrameAction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    if (!d->currentFrame) return ;
    Commands::ItemAction::addItemToScene(d->currentFrame, scene(), tr("Add frame or brackets"));
    d->currentFrame = 0 ;
    event->accept();
  }

} // namespace Molsketch
