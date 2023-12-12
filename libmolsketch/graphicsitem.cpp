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
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QUndoCommand>
#include "graphicsitem.h"
#include "molscene.h"
#include "actions/coloraction.h"
#include "actions/linewidthaction.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <actions/rotateaction.h>
#include "scenesettings.h"
#include "settingsitem.h"

#include <actions/zlevelaction.h>

namespace Molsketch {

  class movePointCommand : public QUndoCommand // TODO eliminate transformcommand or merge
      // TODO Commands::MoveItem seems to be for the same purpose
  {
  private:
    int index ;
    QPointF shift ;
    QSet<graphicsItem *> items ;
  public:
    movePointCommand(const int& i,
                     const QPointF& p,
                     QSet<graphicsItem *> a,
                     QUndoCommand *parent = 0)
      : QUndoCommand(parent),
        index(i),
        shift(p), // if index <0: implicitly the position of the first point
        items(a)
    {
      setText(QObject::tr("Move"));
    }
    void redo() override
    {
      for(auto item : items)
        item->movePointBy(shift, index);

      shift *= -1;
    }
    void undo() override
    {
      redo() ;
    }
    bool mergeWith(const QUndoCommand *other) override
    {
      const movePointCommand* mpc = dynamic_cast<const movePointCommand*>(other) ;
      if (!mpc) return false ;
      if (mpc->items != items)  return false ;
      if (mpc->index != index) return false ;
      shift += mpc->shift;
      return true ;
    }
    int id() const override { return 5000; } // TODO define command IDs in central place
  };

  class graphicsItem::privateData
  {
  public:
    int selectedPoint;
    bool hovering;
    privateData() : selectedPoint(-1), hovering(false) {}
  };

  graphicsItem::graphicsItem(QGraphicsItem *parent)
    : QGraphicsItem(parent),
      lineWidthScaling(1),
      d(new privateData)
  {
    //    setCacheMode(QGraphicsItem::NoCache);
    setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  }

  graphicsItem::graphicsItem(const graphicsItem &other)
    : QGraphicsItem(0),
      m_color(other.m_color),
      lineWidthScaling(other.lineWidthScaling),
      d(new privateData)
  {
    setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  }

  graphicsItem::~graphicsItem()
  {
    delete d;
  }

  void graphicsItem::setColor(const QColor& color)
  {
    m_color = color ;
  }

  QColor graphicsItem::getColor() const
  {
    return m_color ;
  }

  void graphicsItem::setRelativeWidth(const double &w)
  {
    lineWidthScaling = w ;
  }

  qreal graphicsItem::lineWidth() const
  {
    qreal lw = lineWidthScaling ;
    MolScene *myScene = qobject_cast<MolScene*>(scene()) ;
    if (myScene) lw *= sceneLineWidth(myScene) ;
    return lw ;
  }

  qreal graphicsItem::relativeWidth() const
  {
    return lineWidthScaling ;
  }

  int closestPoint(const QPointF &position, const QVector<QPointF> &points, qreal maxDistance = INFINITY) {
    int result = -1;
    for (int i = 0; i < points.size(); ++i) {
      qreal distance = QLineF(position, points[i]).length();
      if (distance < maxDistance) {
        maxDistance = distance;
        result = i;
      }
    }
    return result;
  }

  void graphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  { // TODO this is pretty broken! cf. Commands::MoveItem
    QPointF shift = event->scenePos() - event->lastScenePos();
    QSet<graphicsItem*> selection;
    if (d->selectedPoint < 0 && scene()) {
      foreach(QGraphicsItem* gItem, scene()->selectedItems())
        selection << dynamic_cast<graphicsItem*>(gItem);
    }
    selection.remove(0);
    if (selection.isEmpty())
      selection << this;
    // Snap shift to grid
    MolScene *sc = qobject_cast<MolScene*>(scene());
    if (sc && sc->snappingToGrid())
    {
      if (d->selectedPoint < 0)
      {
        QRectF br;
        foreach(graphicsItem* item, selection)
          if (item->parentItem())
            br |= item->mapToParent(item->boundingRect()).boundingRect();
          else
            br |= item->boundingRect(); // TODO this is not accurate for e.g. atoms (should use pos())
        shift = sc->snapToGrid(event->scenePos()) - br.center();
      }
      else
        shift = sc->snapToGrid(event->scenePos()) - getPoint(d->selectedPoint);
    }

    attemptUndoPush(new movePointCommand(d->selectedPoint,
                                         shift,
                                         selection));
    event->accept();
  }

  void graphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
  { // TODO write test for selecting arrow within frame (also: move individual points; move entire items)
    qDebug() << "Mouse press event:" << this << event;
    if (!scene()) return;
    event->ignore();
    d->selectedPoint = -1;
    qreal ownDistance = distanceToClosestMoveablePoint(event->scenePos());
    QList<QGraphicsItem*> itemsAtEvent = scene()->items(event->scenePos());
    for (auto item : itemsAtEvent) {
      auto gItem = dynamic_cast<graphicsItem*>(item);
      if (!gItem) continue;
      if (gItem->distanceToClosestMoveablePoint(event->scenePos()) < ownDistance) return;
    }
    QGraphicsItem::mousePressEvent(event);
    event->accept();
    d->selectedPoint = closestPoint(event->scenePos(), moveablePoints(), pointSelectionDistance());
  }

  void graphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    QGraphicsItem::mouseReleaseEvent(event);
    d->selectedPoint = -1;
    event->accept();
  }

  void graphicsItem::setCoordinates(const QPolygonF &c) {
    setCoordinates((const QVector<QPointF>&) c);
  }

  void graphicsItem::scale(qreal scaling) {
    setCoordinates(coordinates() * scaling);
  }

  void graphicsItem::readAttributes(const QXmlStreamAttributes &attributes)
  {
    readGraphicAttributes(attributes) ;
    setColor(extractColor(attributes)) ;
    if (attributes.hasAttribute("scalingParameter"))
      lineWidthScaling = attributes.value("scalingParameter").toString().toFloat() ;
    else
      lineWidthScaling = 1.0 ;

    if (attributes.hasAttribute("zLevel"))
      setZValue(attributes.value("zLevel").toString().toFloat());

    if (attributes.hasAttribute("coordinates"))
    {
      QPolygonF coords;
      foreach(const QString& coordinate, attributes.value("coordinates").toString().split(";"))
        coords << QPointF(coordinate.section(",",0,0).toDouble(), coordinate.section(",",1,1).toDouble());
      setCoordinates(coords);
    } else if (attributes.hasAttribute("x2") && attributes.hasAttribute("y2")) // legacy atom
      setCoordinates(QPolygonF() << QPointF(attributes.value("x2").toDouble(), attributes.value("y2").toDouble()));
    else if (attributes.hasAttribute("pox") // legacy reaction arrow
             && attributes.hasAttribute("posy")
             && attributes.hasAttribute("endx")
             && attributes.hasAttribute("endy"))
      setCoordinates(QPolygonF()
                     << QPointF(attributes.value("posx").toDouble(),
                                attributes.value("posy").toDouble())
                     << QPointF(attributes.value("endx").toDouble(),
                                attributes.value("endy").toDouble()));
    else if (attributes.hasAttribute("posx") // legacy mechanism arrow
             && attributes.hasAttribute("posy")
             && attributes.hasAttribute("p1x")
             && attributes.hasAttribute("p1y")
             && attributes.hasAttribute("p2x")
             && attributes.hasAttribute("p2y")
             && attributes.hasAttribute("p3x")
             && attributes.hasAttribute("p3y")
             && attributes.hasAttribute("p4x")
             && attributes.hasAttribute("p4y")){
      QPolygonF coords;
      for (int i = 1 ; i <= 4 ; ++i) {
        QString prefix(QString("p") + QString::number(i));
        coords << QPointF(attributes.value(prefix + "x").toDouble(),
                          attributes.value(prefix + "y").toDouble());
      }
      coords.translate(QPointF(attributes.value("posx").toDouble(),
                               attributes.value("posy").toDouble()));
      setCoordinates(coords);
    }
  }

  QXmlStreamAttributes graphicsItem::xmlAttributes() const
  {
    QXmlStreamAttributes attributes = graphicAttributes() ;
    addColor(attributes, getColor());
    attributes.append("scalingParameter", QString::number(lineWidthScaling));
    attributes.append("zLevel", QString::number(zValue()));
    QStringList coords;
    foreach(const QPointF& coordinate, coordinates())
      coords << QString::number(coordinate.x()) + "," + QString::number(coordinate.y());
    attributes.append("coordinates", coords.join(";"));
    return attributes ;
  }

  void graphicsItem::attemptUndoPush(QUndoCommand *command)
  {
    if (!command) return;
    MolScene *molscene = dynamic_cast<MolScene*>(scene());
    if (!molscene || !molscene->stack())
    {
      command->redo();
      delete command;
    }
    else
      molscene->stack()->push(command) ;
  }

  void graphicsItem::attemptBeginMacro(const QString &text)
  {
    MolScene *molscene = dynamic_cast<MolScene*>(scene());
    if (!molscene || !molscene->stack()) return;
    molscene->stack()->beginMacro(text);
  }

  void graphicsItem::attemptEndEndMacro()
  {
    MolScene *molscene = dynamic_cast<MolScene*>(scene());
    if (!molscene || !molscene->stack()) return;
    molscene->stack()->endMacro();
  }

  void graphicsItem::prepareItemContextMenu(QMenu *contextMenu)
  {
    prepareContextMenu(contextMenu);
  }

  qreal graphicsItem::sceneLineWidth(MolScene *scene) const {
    return scene->settings()->bondWidth()->get();
  }

  void graphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu contextMenu;
    prepareItemContextMenu(&contextMenu);
    foreach(QAction* action, contextMenu.actions()) {
      AbstractItemAction *itemAction = dynamic_cast<AbstractItemAction*>(action);
      if (itemAction) itemAction->setItem(this);
    }

    contextMenu.exec(event->screenPos()) ;
    qDebug() << "removing item" ;
    foreach(QAction* action, contextMenu.actions()) {
      AbstractItemAction *itemAction = dynamic_cast<AbstractItemAction*>(action);
      if (itemAction) itemAction->removeItem(this);
    }
    event->accept();
  }

  qreal graphicsItem::distanceToClosestMoveablePoint(const QPointF &position) const {
    QPolygonF points = moveablePoints();
    int closestPointIndex = closestPoint(position, points);
    return closestPointIndex < 0 ? INFINITY : QLineF(points[closestPointIndex], position).length();
  }

  QVariant graphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
  {
    QVariant retVal = QGraphicsItem::itemChange(change, value);
    if (QGraphicsItem::ItemSelectedChange == change)
    {
      if (parentItem() && parentItem()->isSelected())
        retVal.setValue(false);
      if (value.toBool()) {
        foreach(QGraphicsItem* child, childItems())
          child->setSelected(false);
      }
    }
    return retVal;
  }

#ifdef QT_DEBUG
  QDebug operator <<(QDebug debug, const graphicsItem &item){
    return debug << "Item:" << &item
                 << "Type:" << item.xmlName()
                 << "Parent:" << (void*) item.parentItem()
                 << "Pos:"  << item.pos()
                 << "Scene Pos:" << item.scenePos()
                 << "Bounds:" << item.boundingRect()
                    ;
  }
#endif

  void graphicsItem::setCoordinate(const int &index, const QPointF &p)
  {
    QPolygonF oldCoords(coordinates());
    if (index >= 0 && index < oldCoords.size())
      oldCoords[index] = p;
    setCoordinates(oldCoords);
  }

  QPointF graphicsItem::getPoint(const int &index) const
  {
    QPolygonF oldCoords(coordinates());
    if (index < 0 || index >= coordinateCount()) return QPointF();
    return coordinates().at(index);
  }

  QPointF graphicsItem::lastPoint() const
  {
    int cc = coordinateCount();
    if (!cc) return QPointF();
    return getPoint(cc-1);
  }

  QPointF graphicsItem::firstPoint() const
  {
    if (coordinateCount()) return coordinates().first();
    return QPointF();
  }

  int graphicsItem::coordinateCount() const
  {
    return coordinates().size();
  }

  void graphicsItem::swapPoint(const int &index, QPointF &p)
  {
    int cc = coordinateCount();
    if (index < 0 || index >= cc) return;
    QPolygonF oldCoords(coordinates());
    qSwap(p,oldCoords[index]);
  }

  QPolygonF graphicsItem::moveablePoints() const
  {
    return coordinates();
  }

  void graphicsItem::movePointBy(const QPointF &offset, int pointIndex)
  {
    if (-1 == pointIndex)
    {
      moveItemBy(offset);
      return;
    }
    setCoordinate(pointIndex, offset + getPoint(pointIndex));
  }

  void graphicsItem::moveItemBy(const QPointF &offset)
  {
    QPolygonF coords(coordinates());
    coords.translate(offset);
    setCoordinates(coords);
  }



  void graphicsItem::prepareContextMenu(QMenu *contextMenu)
  {
    colorAction *caction = scene()->findChild<colorAction*>();
    lineWidthAction *lwaction = scene()->findChild<lineWidthAction*>();
    ZLevelAction *zLevelAction = scene()->findChild<ZLevelAction*>();
    rotateAction *raction = scene()->findChild<rotateAction*>();
    if (caction)  contextMenu->addAction(caction);
    if (lwaction) contextMenu->addAction(lwaction);
    if (raction)  contextMenu->addAction(raction);
    if (zLevelAction) contextMenu->addAction(zLevelAction);
  }

  QWidget* graphicsItem::getPropertiesWidget()
  {
    return 0;
  }

  QXmlStreamAttributes &graphicsItem::addColor(QXmlStreamAttributes &attributes, const QColor &color) {
    attributes.append("colorR", QString::number(color.red())) ; // TODO convert this into actual child
    attributes.append("colorG", QString::number(color.green())) ;
    attributes.append("colorB", QString::number(color.blue())) ;
    return attributes;
  }

  QColor graphicsItem::extractColor(const QXmlStreamAttributes &attributes) {
    return QColor(attributes.value("colorR").toString().toInt(),
                        attributes.value("colorG").toString().toInt(),
                  attributes.value("colorB").toString().toInt());
  }

  QByteArray graphicsItem::serialize(const QList<const graphicsItem *> items) {
    QByteArray xml;
    QXmlStreamWriter out(&xml);
    out.writeStartDocument();
    if (items.size() != 1) out.writeStartElement("molsketchItems");
    for (auto item : items) if(item) item->writeXml(out);
    out.writeEndDocument();
    return xml;
  }

  QList<graphicsItem *> graphicsItem::deserialize(const QByteArray &input) {
    QXmlStreamReader in(input);
    QList<graphicsItem*> result;
    while (in.readNextStartElement()) {
      XmlObjectInterface *item = produceXmlObject(in.name().toString()); // TODO change to stringref
      if (!dynamic_cast<graphicsItem*>(item)) continue; // TODO test this
      item->readXml(in);
      result << dynamic_cast<graphicsItem*>(item);
    }
    result.removeAll(nullptr);
    return result;
  }

  void graphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget)
    Q_UNUSED(option)
    if (showHoverBox() && d->hovering) {
      painter->save();
      painter->setPen(QPen(Qt::blue, 1, Qt::DotLine));
      painter->drawPath(shape());
      painter->restore();
    }
    QPolygonF points(moveablePoints());
    if (showHoverPoint() && d->selectedPoint >= 0 && d->selectedPoint < points.size()) {
      painter->save();
      painter->setPen(Qt::red);
      painter->drawEllipse(mapFromScene(points[d->selectedPoint]), 5, 5);
      painter->restore();
    }
    // TODO also handle selection
  }

  void graphicsItem::setHovering(bool hovering) {
    d->hovering = hovering;
  }

  bool graphicsItem::isHovering() const {
    return d->hovering;
  }

  void graphicsItem::selectHoverPoint(const QPointF &position) {
    d->selectedPoint = closestPoint(position, moveablePoints(), pointSelectionDistance());
  }

  void graphicsItem::unselectHoverPoint() {
    d->selectedPoint = -1;
  }

  int graphicsItem::roundedZValue() const {
    return qRound(zValue());
  }

  void graphicsItem::setZValue(const int &newZ) {
    QGraphicsItem::setZValue(newZ);
  }

  qreal graphicsItem::pointSelectionDistance() const
  {
    return 5.;
  }

} // namespace

QPolygonF operator *(const QPolygonF &polygon, qreal scaling) {
  QPolygonF result;
  for (auto point : polygon) {
    result << point * scaling;
  }
  return result.translated(polygon.boundingRect().center() - result.boundingRect().center());
}

QPolygonF operator *(qreal scaling, const QPolygonF &polygon) {
  return polygon * scaling;
}
