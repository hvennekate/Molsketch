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
#include "drawaction.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItemGroup>
#include <QtMath>
#include <QMainWindow>
#include "periodictablewidget.h"
#include "bondtypewidget.h"
#include "molecule.h"
#include "molscene.h"
#include "commands.h"
#include "math2d.h"
#include <QDebug>
#include "scenesettings.h"
#include "settingsitem.h"

namespace Molsketch {

  class drawAction::privateData
  {
  public:
    QWidget *dock ;
    periodicTableWidget *periodicTable;
    bondTypeWidget *bondType;
    QGraphicsLineItem hintLine;
    QGraphicsItemGroup hintPointsGroup;
    drawAction *parent;

    privateData(drawAction* p)
      : hintLine(QLineF(0,0,0,0)),
        parent(p)
    {
      hintLine.setAcceptedMouseButtons(Qt::NoButton);
      hintLine.setZValue(-1);
      hintPointsGroup.setAcceptedMouseButtons(Qt::NoButton);
    }

    void initializeHintPoints()
    {
      foreach (QGraphicsItem* point, hintPointsGroup.childItems())
        hintPointsGroup.removeFromGroup(point);
      MolScene *scene = parent->scene();
      if (!scene) return;

      hintPointsGroup.setPos(0,0);
      // Initialize hint point circle
      qreal bondAngle = scene->settings()->bondAngle()->get();
      qreal bondLength = scene->settings()->bondLength()->get();
      for (qreal angle = 0; angle < 360. ; angle += bondAngle)
      {
        QGraphicsEllipseItem* dot = new QGraphicsEllipseItem(-2.5,-2.5,5,5);
        dot->setBrush(Qt::lightGray);
        dot->setPen(Qt::NoPen);
        dot->setPos(QLineF::fromPolar(bondLength, angle).p2());
        hintPointsGroup.addToGroup(dot);
      }
    }

    void removeHintingElements()
    {
      if (hintLine.scene())
        hintLine.scene()->removeItem(&hintLine);
      if (hintPointsGroup.scene())
        hintPointsGroup.scene()->removeItem(&hintPointsGroup);
    }

    QPointF nearestPoint(const QPointF& currentPosition)
    {
      MolScene* scene = parent->scene();
      QPointF nPoint = (scene ? scene->snapToGrid(currentPosition) : currentPosition) ;

      // Check the hinting points
      qreal minDistance = (scene ? scene->settings()->bondLength()->get()/4. : 10.) ;
      foreach(const QGraphicsItem* hintPoint, hintPointsGroup.childItems())
      {
        qreal distance = QLineF(hintPoint->scenePos(), currentPosition).length();
        if (distance > minDistance) continue;
        minDistance = distance;
        nPoint = hintPoint->scenePos();
      }

      Atom* atom = (scene ? scene->atomNear(currentPosition) : 0);

      return atom ? atom->scenePos() : nPoint;
    }

    void performAtomAction(const QPointF& position)
    {
      Atom* atom = parent->scene()->atomAt(position);
      if (atom)
        parent->attemptUndoPush(new Commands::ChangeElement(atom, periodicTable->currentElement(), tr("change element")));
      else
        Commands::ItemAction::addItemToScene(new Molecule(QSet<Atom*>() << new Atom(position, periodicTable->currentElement(), parent->scene()->settings()->autoAddHydrogen()->get()), QSet<Bond*>()),
                                  parent->scene(), tr("add atom"));
    }

    void performDiatomicAction(const QPointF& posA, const QPointF& posB)
    {
      parent->attemptBeginMacro(tr("draw bond"));
      setOrReplaceBond(findOrCreateAtom(posA), findOrCreateAtom(posB));
      parent->attemptEndMacro();
    }

    Atom* findOrCreateAtom(const QPointF pos)
    {
      Atom* atom = parent->scene()->atomAt(pos);
      if (atom) return atom;
      return new Atom(pos, periodicTable->currentElement(), parent->scene()->settings()->autoAddHydrogen()->get());
    }

    void setOrReplaceBond(Atom* atomA, Atom* atomB)
    {
      Bond *bond = atomA->bondTo(atomB);
      if (bond)
      {
        parent->attemptUndoPush(new Commands::SetBondType(bond, bondType->bondType(), tr("change bond type")));
        if (bondType->backward())
          parent->attemptUndoPush(new Commands::SwapBondAtoms(bond, tr("flip bond")));
      }
      else
        addBondBetween(atomA, atomB);
    }

    void addBondBetween(Atom* atomA, Atom* atomB)
    {
      forceIntoSameMolecule(atomA, atomB);
      addBond(atomA, atomB);
    }

    void forceIntoSameMolecule(Atom*& atomA, Atom*& atomB)
    {
      Molecule* molA = atomA->molecule();
      Molecule* molB = atomB->molecule();
      if (!molA && !molB)
      {
        Commands::ItemAction::addItemToScene(new Molecule(QSet<Atom*>() << atomA << atomB, QSet<Bond*>()), parent->scene(), tr("add molecule"));
        return;
      }
      if (!molA)
      {
        Commands::addItemToMolecule(atomA, molB, parent->scene(), tr("Add atom"));
        return;
      }
      if (!molB)
      {
        Commands::addItemToMolecule(atomB, molA, parent->scene(), tr("add atom"));
        return;
      }
      mergeMolecules(molA, molB, atomA, atomB);
    }

    void mergeMolecules(Molecule *molA, Molecule *molB, Atom*& atomA, Atom*& atomB)
    {
      if (molA == molB) return;
      QMap<Atom*,Atom*> atomMapping;
      Molecule *newMolecule = Molecule::combineMolecules(QSet<Molecule*>() << molA << molB, &atomMapping, 0);
      atomA = atomMapping[atomA];
      atomB = atomMapping[atomB];
      Commands::ItemAction::removeItemFromScene(molA);
      Commands::ItemAction::removeItemFromScene(molB);
      Commands::ItemAction::addItemToScene(newMolecule, parent->scene());
    }

    void addBond(Atom* atomA, Atom* atomB)
    {
      if (bondType->backward()) qSwap(atomA, atomB);
      Commands::addItemToMolecule(new Bond(atomA, atomB, bondType->bondType()), atomA->molecule(), parent->scene());
      // TODO test
    }
  };

  drawAction::drawAction(MolScene *scene)
    : genericAction(scene),
      d(new privateData(this))
  {
    d->dock = new QWidget(qobject_cast<QWidget*>(parent())) ;
    QVBoxLayout *layout = new QVBoxLayout(d->dock) ;
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop) ;
    d->periodicTable = new periodicTableWidget(d->dock) ;
    d->bondType = new bondTypeWidget(true, d->dock) ;
    d->dock->layout()->addWidget(d->periodicTable);
    d->dock->layout()->addWidget(d->bondType);
    connect(d->periodicTable, SIGNAL(elementChanged(QString)),
            this, SLOT(refreshIcon()));
    connect(d->bondType, SIGNAL(currentTypeChanged(int)),
            this, SLOT(refreshIcon()));
    connect(this, SIGNAL(toggled(bool)),
            this, SLOT(toggleVisibility(bool)));
    connect(d->bondType, SIGNAL(currentTypeChanged(int)),
            this, SLOT(refreshIcon()));
    refreshIcon();
    d->dock->setWindowFlags(Qt::Tool);
    d->dock->setWindowTitle(tr("Draw mode"));
    d->dock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // TODO minimize size
    d->dock->hide();
    setText(tr("Draw"));
    setObjectName("draw-action");
  }

  drawAction::~drawAction()
  {
    delete d->dock ;
    delete d ;
  }

  void drawAction::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    if (Qt::LeftButton != event->button()) return ;
    if (event->modifiers()) return ; // TODO change for "snap to grid"
    // TODO delete action

    // Get the position
    QPointF downPos = event->buttonDownScenePos(event->button());

    // TODO handle group abbreviations (Ts, Bn, Bz, tBu, iBu, nBu...) -> settings dialog, settings per file (to allow for different conventions)

    // TODO move to bond!!
    //    Bond *clickedBond = scene()->bondAt(downPos);
//    Atom *clickedAtom = scene()->atomAt(downPos);

    // TODO Check for bond click

    // Show hinting
    // hint points
    Atom* hintAtom = scene()->atomNear(downPos);
    d->initializeHintPoints();
    d->hintPointsGroup.setPos(hintAtom
                              ? hintAtom->scenePos()
                              : scene()->snapToGrid(downPos));
    scene()->addItem(&(d->hintPointsGroup)); // TODO attn: scene takes ownership...

    // hint line
    d->hintLine.setLine(QLineF(downPos, event->scenePos()));
    scene()->addItem(&(d->hintLine)); // TODO attn: scene takes ownership...
    d->hintLine.setVisible(true);
    event->accept();
  }

  void drawAction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  {
     // Check hinting conditions
    if (!(event->buttons() & Qt::LeftButton)) return;

    // Set hinting
    d->hintLine.setLine(QLineF(
                          d->nearestPoint(event->buttonDownScenePos(Qt::LeftButton)),
                          d->nearestPoint(event->scenePos())));
    event->accept();
  }

  // TODO test cases: insert single atom on blank
  //                  insert atom with existing beginAtom
  //                  insert atom with existing endAtom
  //                  link two existing atoms from different molecules
  //                  link two existing atoms from the same molecule
  //                  single atom with already existing atom
  //                  already existing bond
  void drawAction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    QPointF pressPos = d->nearestPoint(event->buttonDownScenePos(event->button()));
    QPointF releasePos = d->nearestPoint(event->scenePos());
    d->removeHintingElements();

    if (pressPos == releasePos)
      d->performAtomAction(pressPos);
    else
      d->performDiatomicAction(pressPos, releasePos);

    scene()->update();
    event->accept();
  }

  void drawAction::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
  {// TODO make part of mouseRelease (see above)
    event->accept();
    QUndoStack *stack = undoStack();
    QPointF downPos = event->buttonDownScenePos(event->button());

    //clicking on an atom spawns a new bond from that atom
    if (scene()->atomAt (downPos)) {
      QPointF new_atom_pos = downPos;
      Atom *at1 = scene()->atomAt (downPos);
      switch (at1 ->numBonds()) {
        case 0:
          {
            qreal x = new_atom_pos.x ();
            new_atom_pos.setX (x + (scene() ? scene()->settings()->bondLength()->get() : 40));
            break;
          }
        case 1:
          {
            Atom *at2 = at1 ->neighbours()[0];
            if (at2 ->neighbours() .size ()< 2) {
              QPointF v = downPos - at1 ->neighbours()[0] ->pos ();

              QPointF rotated_v (v.x()*0.5 - v.y()*sqrt(3)*0.5, v.x()*0.5*sqrt(3) + v.y () *0.5);
              qreal mod = sqrt (rotated_v.x()*rotated_v.x() + rotated_v.y()*rotated_v.y());
              rotated_v *= scene()->settings()->bondLength()->get() / mod;
              new_atom_pos = rotated_v + downPos;
            } else {
              Atom *at3 = at2 ->neighbours()[0];
              if (at3 == at1) at3 = at2 ->neighbours()[1];
              QPointF rotated_v = at2 ->pos () - at3 ->pos ();
              qreal mod = sqrt (rotated_v.x()*rotated_v.x() + rotated_v.y()*rotated_v.y());
              rotated_v *= scene()->settings()->bondLength()->get() /mod;

              new_atom_pos = rotated_v + downPos;
            }
          }
          break;
        case 2:
          {
            Atom *n1 = at1 ->neighbours()[0];
            Atom * n2 = at1 ->neighbours()[1];
            QPointF v1 = n1 ->pos ();
            QPointF v2 = n2 ->pos ();
            QPointF v3 = v1 + v2;
            QPointF v4 (v3.x () / 2, v3.y () / 2);
            QPointF v5 =  at1 ->pos () - v4;
            qreal mod = sqrt (v5.x()*v5.x() + v5.y()*v5.y());
            v5 = QPointF (v5.x()/mod * scene()->settings()->bondLength()->get(),
                          v5.y()/mod * scene()->settings()->bondLength()->get());
            new_atom_pos = v5 + at1->pos();
          }
          break;
        default:
          break;
      }
      if (new_atom_pos != downPos) {
        stack->beginMacro("Add Bond");
        Atom* atom = new Atom(new_atom_pos,d->periodicTable->currentElement(), scene()->settings()->autoAddHydrogen()->get());
        Commands::addItemToMolecule(atom, at1->molecule(), scene());
        if (d->bondType->backward()) qSwap(at1, atom);
        Bond* bond = new Bond(at1, atom, d->bondType->bondType());
        Commands::addItemToMolecule(bond, at1->molecule(), scene());
        stack->endMacro();
      }
    }
  }

  void drawAction::wheelEvent(QGraphicsSceneWheelEvent *event)
  {
    if (!event->delta()) return;
    d->bondType->cycleTypes(event->delta() > 0); // TODO size of wheel movement
    event->accept();
  }

  void drawAction::refreshIcon()
  {// TODO redo
    QPixmap elementIcon = d->periodicTable->currentIcon(),
        bondIcon = d->bondType->currentIcon();
    QSize elementSize = elementIcon.size();
    QSize bondSize = bondIcon.size();
    QSize totalSize(bondSize + elementSize);
    totalSize.setWidth(qMax(totalSize.width(), totalSize.height()));
    totalSize.setHeight(totalSize.width());
    QPixmap pixmap(totalSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.drawPixmap(0, elementSize.height(), bondIcon);
    painter.drawPixmap(bondSize.width(), 0, elementIcon);
    setIcon(pixmap);
    /*
    QIcon bondIcon(d->bondType->bondIcon()) ;
    QString element = d->periodicTable->currentElement();
    int height = 0, width = 0;
    foreach (const QSize& size, bondIcon.availableSizes())
    {
      if (size.height() > height)
      {
        height = size.height();
        width  = size.width();
      }
    }
    QFont atomFont(font()); // TODO measuring is not quite right
    atomFont.setPixelSize(height);
    QFontMetrics metrics(atomFont);
    QRectF atomRect(QFontMetrics(atomFont).boundingRect(element));
    QImage compoundIcon(width + atomRect.width(), qMax((qreal) height, atomRect.height()), QImage::Format_RGB888);
    compoundIcon.fill(parentWidget() ? parentWidget()->palette().window().color() : Qt::white);
    QPainter painter(&compoundIcon);
    bondIcon.paint(&painter,0,.5*(compoundIcon.height()-height),width,height);
    painter.setFont(atomFont);
    //    painter.drawText(width,atomRect.height(),element);
    atomRect.moveTopLeft(QPointF(width,0));
    qDebug() << "AtomRect" << atomRect << atomRect.height()<< width << compoundIcon.size();
    painter.drawText(atomRect, element);
    setIcon(QPixmap::fromImage(compoundIcon));*/
  }

  void drawAction::toggleVisibility(bool visible)
  {
    if (visible) d->dock->show();
    else d->dock->hide();
    if (!visible) d->removeHintingElements();
  }

} // namespace Molsketch

