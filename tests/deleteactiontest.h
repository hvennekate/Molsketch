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

#include <cxxtest/TestSuite.h>
#include "utilities.h"

#include <QSet>
#include <molecule.h>
#include <molscene.h>
#include <QDebug>
#include <arrow.h>
#include <frame.h>

#include <actions/deleteaction.h>

using namespace Molsketch;

class DeleteActionTest : public CxxTest::TestSuite {
  MolScene *scene;
  Atom *atomA, *atomB, *atomC;
  Bond *bondA, *bondB;
  Molecule *molecule;
  Arrow *arrow;
  Frame *frame;
  deleteAction *action;
  QList<QGraphicsItem*> originalItems;
public:
  void setUp() {
    atomA = new Atom(QPointF(1,1));
    atomB = new Atom(QPointF(2,2));
    atomC = new Atom(QPointF(3,3));

    bondA = new Bond(atomA, atomB);
    bondB = new Bond(atomB, atomC);

    molecule = new Molecule(QSet<Atom*>() << atomA << atomB << atomC, QSet<Bond*>() << bondA << bondB);

    arrow = new Arrow();
    frame = new Frame();

    scene = new MolScene;
    scene->addItem(molecule);
    scene->addItem(arrow);
    scene->addItem(frame);

    action = new deleteAction(scene);
    originalItems = scene->items();
  }

  void tearDown() {
    if (scene->stack()->canUndo()) scene->stack()->undo();
    QS_ASSERT_EQUALS(toSet(scene->items()), toSet(originalItems));
    QS_ASSERT_EQUALS(toSet(molecule->atoms()), QSet<Atom*>() << atomA << atomB << atomC);
    QS_ASSERT_EQUALS(toSet(molecule->bonds()), QSet<Bond*>() << bondA << bondB);
    QS_ASSERT_EQUALS(bondA->beginAtom(), atomA);
    QS_ASSERT_EQUALS(bondA->endAtom(), atomB);
    QS_ASSERT_EQUALS(bondB->beginAtom(), atomB);
    QS_ASSERT_EQUALS(bondB->endAtom(), atomC);
    delete scene;
  }

  void testNoItemsSelectedDoesNothing() {
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size());
  }

  void testDeletingAtom() {
    atomA->setSelected(true);
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size() - 2);
    TS_ASSERT(!scene->items().contains(atomA));
    TS_ASSERT(!scene->items().contains(bondA));
  }

  void testDeletingBond() {
    bondA->setSelected(true);
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size());
    TS_ASSERT(!scene->items().contains(bondA));
  }

  void testDeletingBondAndAtom() {
    atomA->setSelected(true);
    bondA->setSelected(true);
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size() - 2);
    TS_ASSERT(!scene->items().contains(atomA));
    TS_ASSERT(!scene->items().contains(bondA));
  }

  void testDeletingTwoBondsAndTwoAtoms() {
    atomA->setSelected(true);
    bondA->setSelected(true);
    bondB->setSelected(true);
    atomC->setSelected(true);
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size() - 4);
    TS_ASSERT(!scene->items().contains(atomA));
    TS_ASSERT(!scene->items().contains(bondA));
    TS_ASSERT(!scene->items().contains(bondB));
    TS_ASSERT(!scene->items().contains(atomC));
  }

  void testDeletingArrow() {
    arrow->setSelected(true);
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size() - 1);
    TS_ASSERT(!scene->items().contains(arrow));
  }

  void testDeletingFrame() {
    frame->setSelected(true);
    action->trigger();
    TS_ASSERT_EQUALS(scene->items().size(), originalItems.size() - 1);
    TS_ASSERT(!scene->items().contains(frame));
  }
};
