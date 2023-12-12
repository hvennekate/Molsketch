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
#include "rotateaction.h"
#include "QUndoCommand"
#include "molscene.h"
#include "transformcommand.h"
#include "iconutils.h"

#include <QGraphicsSceneMouseEvent>
#include <QToolTip>

namespace Molsketch {

  struct transformAction::privateData
  {
    QList<graphicsItem*> transformItems;
    QLineF originalLine ;
  };

  transformAction::transformAction(MolScene *scene) :
    genericAction(scene),
    d(new privateData)
  {
  }

  void transformAction::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    if (event->button() != Qt::LeftButton) return ;
    // Get items (from selection or by being clicked on
    QList<QGraphicsItem*> items = scene()->selectedItems();
    if (items.isEmpty())
      items << scene()->itemAt(event->buttonDownScenePos(Qt::LeftButton), QTransform());
    // Get items we actually care about
    d->transformItems.clear();
    foreach(QGraphicsItem* item, items)
    {
      graphicsItem *gItem = dynamic_cast<graphicsItem*>(item);
      if (!gItem) continue;
      if (gItem->type() == graphicsItem::AtomType
          || gItem->type() == graphicsItem::BondType)
        gItem = dynamic_cast<graphicsItem*>(gItem->parentItem());
      if (!gItem) continue;
      if (!d->transformItems.contains(gItem))
        d->transformItems << gItem;
    }
    if (d->transformItems.isEmpty()) return;

    // Get bounding rect of all items
    QRectF combinedBoundingRect = d->transformItems.first()->boundingRect();
    foreach(graphicsItem* item, d->transformItems)
      combinedBoundingRect |= item->boundingRect();

    d->originalLine = QLineF(combinedBoundingRect.center(),
                            event->buttonDownScenePos(Qt::LeftButton)) ;
    event->accept();
  }

  void transformAction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  {
    if (d->transformItems.isEmpty()) return;

    // Calculate the rotation angle
    QLineF newLine(d->originalLine.p1(), event->scenePos()) ;
    if (event->modifiers() & Qt::ShiftModifier)
      snapLine(QLineF(d->originalLine.p1(), event->buttonDownScenePos(Qt::LeftButton)), newLine);

    QToolTip::showText(event->screenPos(),
                       cursorLabel(QLineF(d->originalLine.p1(), event->buttonDownScenePos(Qt::LeftButton)), newLine),
                       qobject_cast<QWidget*>(parent()),
                       QRect());
    transformCommand *cmd = new transformCommand(d->transformItems,
                                                 generateTransform(d->originalLine,
                                                                   newLine), // TODO doesn't work
                                                 d->originalLine.p1());
    cmd->setText(text());
    attemptUndoPush(cmd);
    d->originalLine = newLine ;
    event->accept();
  }

  void transformAction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    if (d->transformItems.isEmpty()) return;
    event->accept();
    d->transformItems.clear();
  }

  QString transformAction::cursorLabel(const QLineF &originalLine, QLineF &currentLine)
  {
    Q_UNUSED(originalLine)
    Q_UNUSED(currentLine)
    return QString();
  }

  rotateAction::rotateAction(MolScene *scene)
    : transformAction(scene)
  {
    setIcon(getInternalIcon("rotate")) ;
    setText(tr("Rotate")) ;
  }

  void rotateAction::snapLine(const QLineF &originalLine, QLineF &currentLine)
  {
    const qreal step = (scene() ? scene()->bondAngle() : 30);
    qreal angle = step*qRound(currentLine.angleTo(originalLine)/step);
    currentLine.setAngle(originalLine.angle()-angle);
  }

  QString rotateAction::cursorLabel(const QLineF &originalLine, QLineF &currentLine)
  {
    return QString::number(currentLine.angleTo(originalLine)) + tr("°\nHold Shift to use fixed angles.");
  }

  QTransform rotateAction::generateTransform(const QLineF &originalLine, const QLineF &currentLine)
  {
    return QTransform().rotate(currentLine.angleTo(originalLine));
  }

  translateAction::translateAction(MolScene *scene)
    : transformAction(scene)
  {
    setIcon(getInternalIcon("translate")) ;
    setText(tr("Move")) ;
  }

  QTransform translateAction::generateTransform(const QLineF &originalLine, const QLineF &currentLine)
  {
    QPointF diff = currentLine.p2() - originalLine.p2() ;
    return QTransform().translate(diff.x(), diff.y()) ;
  }

  void translateAction::snapLine(const QLineF& originalLine, QLineF &currentLine)
  {
    Q_UNUSED(originalLine)
    Q_UNUSED(currentLine)
    // TODO
  }

} // namespace Molsketch
