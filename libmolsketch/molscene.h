/***************************************************************************
 *   Copyright (C) 2007 by Harm van Eersel                                 *
 *   Copyright (C) 2009 by Tim Vandermeersch                               *
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

#ifndef MSK_MOLSCENE_H
#define MSK_MOLSCENE_H

#include <QGraphicsScene>
#include <QUndoCommand>

#include "bond.h"
#include "abstractxmlobject.h"
#include "stringify.h"

class QString;
class QImage;
class QListWidgetItem;
class QTableWidgetItem;
class QUndoStack;

namespace Molsketch {

  class Molecule;
  class Atom;
  class Bond;
  class TextInputItem;
  class genericAction;
  class SceneSettings;

  class MolScene : public QGraphicsScene, public abstractXmlObject
  {
    Q_OBJECT

    public:
    static const QString mouseWheelForCyclingTools;
      enum RenderMode
      {
        RenderLabels,
        RenderColoredSquares,
        RenderColoredCircles,
        RenderColoredWireframe,
      };

      explicit MolScene(QObject* parent = 0);
      /** Creates a new MolScene with @p settings. Will assume ownership of @p settings! */
      MolScene(SceneSettings *settings, QObject* parent = 0);
      ~MolScene();

      SceneSettings* settings() const;

      static QString mimeType();

      qreal bondAngle() const;

      /**
       * @return The current RenderMode. Default is RenderLabels
       */
      RenderMode renderMode() const; // TODO do we really need this?
      /**
       * Set the RenderMode.
       */
      void setRenderMode(RenderMode mode);

      // Commands
      /** Renders the @p rect on the scene in a image. */
      QImage renderImage(const QRectF &rect, const qreal &scalingFactor = 10);

      QByteArray toSvg();


      QUndoStack *stack() const;

      QList<Molsketch::genericAction *> sceneActions() const;

      // Advanced queries
      /** Returns the first molecule at position @p pos or NULL if none. */
      Molecule* moleculeAt(const QPointF &pos);
      /** Returns the first atom at position @p pos or NULL if none. */
      Atom* atomAt(const QPointF &pos);
      /** Returns the first bond at position @p pos or NULL if none. */
      Bond* bondAt(const QPointF &pos);

      /** Calculates the nearest magnetic point around @p curPos. */
      QPointF nearestPoint(const QPointF &curPos);

      TextInputItem *inputItem() ;

      QList<Atom *> atoms() const;
      QList<Molecule *> molecules() const;

    signals:
      void copyAvailable(bool);
      void pasteAvailable(bool);
      void documentChange( );

    public slots:
      /** Slot to cut the current selection to the clipboard. */
      void cut();
      /** Slot to copy the current selection to the clipboard. */
      void copy();
      /** Slot to paste the current clipboard contents. */
      void paste();
      /** Slot to clear the scene. */
      void clear();
      /** Slot to select all contents of the scene. */
      void selectAll();
      void setGrid(bool on = true);
      void clipboardChanged();


#ifdef QT_DEBUG
  void debugScene();
#endif


    protected:
      void keyPressEvent(QKeyEvent* keyEvent) override;
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
      void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
      void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
      void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

      void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
      void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
      void dropEvent(QGraphicsSceneDragDropEvent *event) override;
      void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;

      void wheelEvent(QGraphicsSceneWheelEvent *event) override;
  public:

      QString xmlName() const override;
      static QString xmlClassName();
      QPointF snapToGrid(const QPointF& point, bool force = false);
      bool snappingToGrid() const;
      bool cyclingByMouseWheelEnaled() const;
      Atom *atomNear(const QPointF &pos, qreal tolerance = 10.0);
      template <class T>
      T *itemNear(const QPointF &pos, qreal tolerance = 10.0) { // TODO unit test
        qreal minDistance = tolerance;
        T *result = nullptr;
        for(QGraphicsItem* item : items(QRectF(pos.x()-tolerance, pos.y()-tolerance, tolerance, tolerance))) {
          qreal distance = QLineF(item->boundingRect().center(), pos).length();
          if (T *itemPointer = dynamic_cast<T*>(item)) {
            if (distance < minDistance) {
              result = itemPointer;
              minDistance = distance;
            }
          }
        }
        return result;
      }
      QWidget *getPropertiesWidget(); // TODO there should be an intgerface for this
  protected:
      XmlObjectInterface *produceChild(const QString &childName, const QXmlStreamAttributes &attributes) override;
      QList<const XmlObjectInterface *> children() const override;
      void readAttributes(const QXmlStreamAttributes &attributes) override;
      QXmlStreamAttributes xmlAttributes() const override;
      void afterReadFinalization() override;

  private:

      RenderMode m_renderMode;
      class privateData;
      privateData *d;

  private slots:
      void updateAll() ;
      void selectionSlot();
      void updateGrid(const QRectF &newSceneRect);
  };

} // namespace

#endif
