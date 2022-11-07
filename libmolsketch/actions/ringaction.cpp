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
#include "ringaction.h"

#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <bond.h>
#include <QtMath>
#include <commands.h>
#include "molscene.h"
#include "math2d.h"
#include "molecule.h"
#include "iconutils.h"

#include "scenesettings.h"
#include "settingsitem.h"

#include <QDebug>

namespace Molsketch {

  class ringAction::privateData
  {
  public:
    QGraphicsPolygonItem hintMoleculeItems;
    QPolygonF hintRingPoints;
    ringAction *parent;

    privateData(ringAction* p) :
      hintMoleculeItems(0),
      parent(p)
    {
      hintMoleculeItems.hide();
      hintMoleculeItems.setPen(QPen(Qt::lightGray));
    }

    void createHintRing(int size, bool aromatic, const MolScene *scene) // TODO use real molecule
    {
      Q_UNUSED(aromatic) // TODO
      if (size < 3) return;

      hintRingPoints.clear();
      qreal radius = (scene ? scene->settings()->bondLength()->get() : 40.) / (2*sin(M_PI/size));
      for (int i = 0; i < size ; ++i)
        hintRingPoints.append(QLineF::fromPolar(radius, 90+(360.*i)/size).p2());

      hintMoleculeItems.setPolygon(hintRingPoints);
      parent->scene()->addItem(&hintMoleculeItems); // TODO scene takes ownership
    }

    void alignRingWithAtom(Atom *atom) // TODO partial merge with alignRingWithBond()
    {
      if (hintRingPoints.size() < 2) return;

      QPointF moleculeNormal = QPointF(0.0, -1.0);
      QPointF ringNormal = normalized(hintRingPoints.first());
      QPointF rp = hintRingPoints.first();
      if (atom->numBonds())
      {
        // rotate/translate ring to align with the bond
        moleculeNormal = atom->scenePos() - atom->neighbours().first()->scenePos();
        if (atom->numBonds() > 1)
          moleculeNormal += atom->scenePos() - atom->neighbours()[1]->scenePos();
        moleculeNormal = normalized(moleculeNormal);
      }

      qreal ang = angle(moleculeNormal, ringNormal) * 180.0 / M_PI;
      if (angleSign(moleculeNormal, ringNormal) > 0.0)
        ang = -ang;
      hintMoleculeItems.setTransform(
            QTransform().rotate(ang+180.0).translate(-rp.x(), -rp.y()));
      if (atom->numBonds()) hintMoleculeItems.setPos(atom->scenePos());
    }

    void alignRingWithBond(Bond *bond, const QPointF &pos)
    {
      Q_CHECK_PTR(bond->beginAtom());
      Q_CHECK_PTR(bond->endAtom());

      if (hintRingPoints.size() < 2) return;

      // scale to match bond length
      // just translate ring to make 1 atom align
      QPointF bondNormal = normalized(bond->endAtom()->pos() - bond->beginAtom()->pos());
      QPointF ringNormal = normalized( (hintRingPoints[0] + hintRingPoints[1]) / 2.0 );
      QPointF rp = hintRingPoints.first();

      qreal ang = angle(bondNormal, ringNormal) * 180.0 / M_PI;
      double angSign = angleSign(bondNormal, ringNormal);
      if (angSign > 0.0)
        ang = -ang;

      double triSign = triangleSign(bond->beginAtom()->scenePos(), bond->endAtom()->scenePos(), pos);

      qreal bondLengthScale = QLineF(bond->beginAtom()->pos(), bond->endAtom()->pos()).length()/40.; // TODO magic number 40
      if (triSign > 0.0) {
        hintMoleculeItems.setTransform(QTransform()
                                       .scale(bondLengthScale, bondLengthScale)
                                       .rotate(ang+270.0)
                                       .translate(-rp.x(), -rp.y())
                                       );
        hintMoleculeItems.setPos(bond->endAtom()->scenePos());
      } else {
        hintMoleculeItems.setTransform(QTransform()
                                       .scale(bondLengthScale, bondLengthScale)
                                       .rotate(ang+90.0)
                                       .translate(-rp.x(), -rp.y())
                                       );
        hintMoleculeItems.setPos(bond->beginAtom()->scenePos());
      }
    }
  };

#define ADDRINGSUBACTION(NAME, SIZE) \
  c = (SIZE > 0 ? "C" : "a"); \
  newAction = new QAction(getInternalIcon(c + "1" + c.repeated(qAbs(SIZE) -1) + "1"), tr(NAME), this); \
  newAction->setData(SIZE); \
  addSubAction(newAction);

  ringAction::ringAction(MolScene *scene)
    : multiAction(scene),
      d(new privateData(this))
  {
    setText(tr("Ring"));
    setObjectName("ring-action");
    QAction *newAction = 0 ;
    QString c;
    ADDRINGSUBACTION("Cyclopropyl", 3);
    ADDRINGSUBACTION("Cyclobutyl", 4);
    ADDRINGSUBACTION("Cyclopentyl", 5);
    ADDRINGSUBACTION("Cyclohexyl", 6);
    ADDRINGSUBACTION("Cycloheptyl", 7);
    ADDRINGSUBACTION("Cyclooctyl", 8);
    ADDRINGSUBACTION("Cyclopentadienyl", -5);
    ADDRINGSUBACTION("Aryl group", -6);
    connect(this, SIGNAL(changed()), this, SLOT(changeRing()));
    changeRing();
  }

  ringAction::~ringAction()
  {
    if(d->hintMoleculeItems.scene()) d->hintMoleculeItems.scene()->removeItem(&(d->hintMoleculeItems));
    delete d;
  }

  qreal pointLength(const QPointF& p)
  {
    return sqrt(pow(p.x(),2) + pow(p.y(),2));
  }

  void ringAction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
  {
    event->accept();
    d->hintMoleculeItems.show();
    d->hintMoleculeItems.setPos(event->scenePos()); // TODO snap vertices to grid instead of center
    // optimize position on grid
    if (scene()->snappingToGrid())
    {
      QList<QPointF> vectors;
      QList<int> counts;
      foreach(QPointF point, d->hintMoleculeItems.polygon().translated(d->hintMoleculeItems.pos()))
      {
        QPointF vector = scene()->snapToGrid(point)-point;
        if (vectors.contains(vector)) ++ counts[vectors.indexOf(vector)];
        else { vectors << vector; counts << 1;}
      }
      int maxIndex = 0;
      for (int i = 0 ; i < counts.size() ; ++i)
        if (counts[i] > counts[maxIndex] ||
            (counts[i] == counts[maxIndex]
                 && pointLength(vectors[i]) < pointLength(vectors[maxIndex])))
           maxIndex = i;
      d->hintMoleculeItems.moveBy(vectors[maxIndex].x(), vectors[maxIndex].y());
    }
    d->hintMoleculeItems.setTransform(QTransform()); // TODO get rid of transforms

    // Get the position
    QPointF downPos = event->scenePos();

    Atom *atom = scene()->atomNear(downPos);
    if (atom)
      d->alignRingWithAtom(atom);

    Bond *bond = scene()->bondAt(downPos);
    if (bond)
      d->alignRingWithBond(bond, downPos);
  }

  QList<Atom *> ringAction::getRingAtoms(Molecule* newMolecule)
  {
    QList<Atom*> ringAtoms;
    foreach(const QPointF& hintPoint, d->hintRingPoints)
    {
      QPointF vertex(d->hintMoleculeItems.mapToScene(hintPoint));
      Atom *atom = scene()->atomAt(vertex);
      if (atom && atom->molecule())
      {
        Molecule* otherMolecule = atom->molecule();
        if (otherMolecule != newMolecule) // merge other molecule
        {
          QMap<const Atom*, Atom*> atomMapping;
          foreach(const Atom* atom, otherMolecule->atoms())
            atomMapping[atom] = newMolecule->addAtom(new Atom(*atom));
          foreach(const Bond* bond, otherMolecule->bonds())
            newMolecule->addBond(atomMapping[bond->beginAtom()], atomMapping[bond->endAtom()], bond->bondType(), bond->getColor());
          atom = atomMapping[atom];
          Commands::ItemAction::removeItemFromScene(otherMolecule);
        }
      }
      else
      {
        atom = new Atom(vertex, "C", scene()->settings()->autoAddHydrogen()->get());
        newMolecule->addAtom(atom);
      }
      ringAtoms << atom;
    }
    return ringAtoms;
  }

  QList<Bond *> ringAction::getRingBonds(QList<Atom*> ringAtoms, Molecule* newMolecule)
  {
    QList<Bond*> ringBonds;
    for (int i = 0 ; i < ringAtoms.size() ; ++i)
    {
      Atom *atom = ringAtoms[i];
      Atom *next = ringAtoms[(ringAtoms.size() == i+1) ? 0 : i+1];
      Bond* bond = newMolecule->bondBetween(atom, next);
      if (!bond) bond = newMolecule->addBond(atom, next);
      ringBonds << bond;
    }

    return ringBonds;
  }

  void ringAction::addAromaticity(QList<Bond*> ringBonds)
  {
    if (activeSubAction()->data().toInt() < 0) // Test for aromaticity
    {
      foreach(Bond* bond, ringBonds)
      {
        if (bond->bondOrder() > 1) continue;
        bool canBeDouble = true;
        foreach(Bond* otherBond, bond->beginAtom()->bonds() + bond->endAtom()->bonds())
          canBeDouble = canBeDouble && otherBond->bondOrder() < 2;
        if (!canBeDouble) continue;
        bond->setType(Bond::DoubleAsymmetric);
      }
    }
  }

  void ringAction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
    event->accept();

    attemptBeginMacro(tr("Add ring"));

    Molecule *newMolecule = new Molecule();
    Commands::ItemAction::addItemToScene(newMolecule, scene());
    QList<Atom*> ringAtoms = getRingAtoms(newMolecule);
    QList<Bond*> ringBonds = getRingBonds(ringAtoms, newMolecule);
    addAromaticity(ringBonds);

    attemptEndMacro();
  }

  void ringAction::leaveSceneEvent(QEvent *event)
  {
    Q_UNUSED(event)
    scene()->removeItem(&(d->hintMoleculeItems));
  }

  void ringAction::enterSceneEvent(QEvent *event)
  {
    Q_UNUSED(event)
    scene()->addItem(&(d->hintMoleculeItems));
  }

  void ringAction::changeRing() // TODO virtual function in multiaction
  {
    if (isChecked() && activeSubAction())
      d->createHintRing(qAbs(activeSubAction()->data().toInt()), false, scene());
    else
      scene()->removeItem(&(d->hintMoleculeItems));
  }

} // namespace Molsketch

