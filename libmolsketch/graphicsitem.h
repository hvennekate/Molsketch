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
#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QGraphicsItem>
#include "abstractxmlobject.h"

class QUndoCommand ;

namespace Molsketch {

  class MolScene ;

  class graphicsItem : public QGraphicsItem, public abstractXmlObject
  {
  public:

    enum Types { // TODO do we really need this?
      MoleculeType = QGraphicsItem::UserType + 1,
      AtomType = QGraphicsItem::UserType + 2,
      BondType = QGraphicsItem::UserType + 3,
      TextInputType = QGraphicsItem::UserType + 5,
      ArrowType = QGraphicsItem::UserType + 8,
      FrameType = QGraphicsItem::UserType + 9,
      LonePairType = QGraphicsItem::UserType + 10,
      RadicalType = QGraphicsItem::UserType + 11
    };

    explicit graphicsItem(QGraphicsItem* parent = 0);
    explicit graphicsItem(const graphicsItem& other);
    ~graphicsItem();
    // TODO XML i/o
    void setColor(const QColor &color) ;
    QColor getColor() const ;
    void setRelativeWidth(const double& w) ;
    qreal lineWidth() const ;
    qreal relativeWidth() const ;

    /** Event handlers */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /** coordinate functions */
    virtual QPolygonF coordinates() const = 0;
    virtual void setCoordinates(const QVector<QPointF>& c) = 0;
    void setCoordinates(const QPolygonF& c);
    void scale(qreal scaling);
    virtual void setCoordinate(const int& index, const QPointF& p);
    virtual QPointF getPoint(const int& index) const;
    virtual QPointF lastPoint() const;
    virtual QPointF firstPoint() const;
    virtual int coordinateCount() const;
    virtual void swapPoint(const int& index, QPointF& p);
    virtual QPolygonF moveablePoints() const;
    qreal distanceToClosestMoveablePoint(const QPointF &position) const;
    virtual void movePointBy(const QPointF& offset, int pointIndex = -1);
    virtual void moveItemBy(const QPointF& offset);

    virtual void prepareContextMenu(QMenu* contextMenu);
    virtual QWidget *getPropertiesWidget();

    static QXmlStreamAttributes& addColor(QXmlStreamAttributes& attributes, const QColor& color);
    static QColor extractColor(const QXmlStreamAttributes& attributes);
    static QByteArray serialize(const QList<const graphicsItem*> items);
    static QList<graphicsItem*> deserialize(const QByteArray& input);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setHovering(bool);
    void selectHoverPoint(const QPointF& position);
    void unselectHoverPoint();
    int roundedZValue() const;
    void setZValue(const int &newZ);
  protected:
    bool isHovering() const;
    qreal pointSelectionDistance() const;
    virtual bool showHoverBox() const { return true; }
    virtual bool showHoverPoint() const { return true; }
    void readAttributes(const QXmlStreamAttributes &attributes) override;
    QXmlStreamAttributes xmlAttributes() const override;
    virtual void readGraphicAttributes(const QXmlStreamAttributes& attributes) { Q_UNUSED(attributes)}
    virtual QXmlStreamAttributes graphicAttributes() const { return QXmlStreamAttributes() ; }
    /**
     * Attempt to push a command onto the undo stack. If none is available, execute the command and dispose of it.
     */
    void attemptUndoPush(QUndoCommand* command) ;
    void attemptBeginMacro(const QString& text);
    void attemptEndEndMacro();
    virtual void prepareItemContextMenu(QMenu* contextMenu);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    template<typename T>

    static QList<T> itemsByType(QList<QGraphicsItem*> input) {
      QList<T> result;
      for (auto item : input)
        if (T typedItem = dynamic_cast<T>(item))
          result << typedItem;
      return result;
    }

    template<typename T>
    QList<T> childrenByType() const {
      return itemsByType<T>(childItems());
    }
  private:
    QColor m_color ;
    qreal lineWidthScaling ;
    class privateData;
    privateData *d;
    virtual qreal sceneLineWidth(MolScene* scene) const;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    // TODO automatically return item children from graphicsitem as xmlitem
  };

#ifdef QT_DEBUG
  QDebug operator <<(QDebug debug, const graphicsItem &item);
#endif

} // namespace

QPolygonF operator *(const QPolygonF &polygon, qreal scaling);
QPolygonF operator *(qreal scaling, const QPolygonF &polygon);

#endif // GRAPHICSITEM_H
