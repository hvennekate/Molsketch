/***************************************************************************
 *   Copyright (C) 2016 by Hendrik Vennekate                               *
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

#include <QSet>
#include <atom.h>
#include <molecule.h>
#include <molscene.h>
#include <molview.h>
#include <cxxtest/TestSuite.h>
#include <actions/drawaction.h>
#include "utilities.h"

using namespace Molsketch;

class DrawActionAcceptanceTest : public CxxTest::TestSuite {

  MolScene *scene;
  MolView *view;
  drawAction *action;

  template<typename T>
  QList<T> sceneItems() {
    QList<T> result;
    for (auto item : scene->items())
      if (T t = dynamic_cast<T>(item))
        result << t;
    return result;
  }

public:
  void setUp() {
    scene = new MolScene;
    view = new MolView(scene);
    action = new drawAction(scene);
  }

  void tearDown() {
    delete view;
    delete scene;
  }

  void testHintPoints() {
    QPointF originalAtom(20,10),
        drawingStart(19,10),
        drawingStop(-19,10),
        targetPosition(-20,10);

    Atom *atom = new Atom(originalAtom, "C");
    Molecule *molecule = new Molecule(QSet<Atom*>() << atom, QSet<Bond*>());
    action->setChecked(true);
    scene->addItem(molecule);

    view->show();

    QTest::mousePress(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(drawingStart));
    QTest::mouseMove(view->viewport(), view->mapFromScene(drawingStop));
    QTest::mouseRelease(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(drawingStop));

    QGraphicsItem* newAtom = scene->itemAt(targetPosition, QTransform());
    TS_ASSERT(newAtom); // TODO macro for assert present or return // TODO QSM_ASSERT with message as QString
    if (!newAtom) return;
    QS_ASSERT_EQUALS(newAtom->pos(), targetPosition);
  }

  void testUndoing() {
    action->setChecked(true);
    view->show();

    QPointF drawingStart(19,10), drawingStop(-19,10);

    QTest::mousePress(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(drawingStart));
    QTest::mouseMove(view->viewport(), view->mapFromScene(drawingStop));
    QTest::mouseRelease(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(drawingStop));

    QS_ASSERT_EQUALS(sceneItems<Bond*>().size(), 1);
    QS_ASSERT_EQUALS(sceneItems<Atom*>().size(), 2);
    QS_ASSERT_EQUALS(toSet(sceneItems<Bond*>().first()->atoms()), toSet(sceneItems<Atom*>()));

    scene->stack()->undo();
    QS_ASSERT_EQUALS(sceneItems<graphicsItem*>().size(), 0);

    scene->stack()->redo();
    QS_ASSERT_EQUALS(sceneItems<Bond*>().size(), 1);
    QS_ASSERT_EQUALS(sceneItems<Atom*>().size(), 2);
    QS_ASSERT_EQUALS(toSet(sceneItems<Bond*>().first()->atoms()), toSet(sceneItems<Atom*>()));
  }
};
