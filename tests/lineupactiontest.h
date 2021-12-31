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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/TestSuite.h>
#include <actions/lineupaction.h>
#include <molecule.h>
#include <molscene.h>
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QRadioButton>
#include "boundingrectassertion.h"
#include "undostackassertion.h"

using namespace BoundingRectAssert;
using namespace UndoStackAssert;
using namespace Molsketch;

class LineUpActionTest : public CxxTest::TestSuite {
  MolScene *scene;
  LineUpAction *action;
  Molecule *m1, *m2;
public:
  void setUp() {
    m1 = new Molecule(QSet<Atom*>{new Atom(QPointF(5,5), "Br")},
                      QSet<Bond*>());
    m2 = new Molecule(QSet<Atom*>{new Atom(QPointF(10,10), "Br")},
                      QSet<Bond*>());
    scene = new MolScene;
    scene->addItem(m1);
    scene->addItem(m2);
  }

  void tearDown() {
    delete scene;
  }

  // TODO generally test the GUI

  void testHorizontalLiningUpWithDistance() {
    qreal distance = 10;
    action = LineUpAction::horizontal(scene);
    scene->selectAll();
    QTimer::singleShot(0, [&] {
      QWidget *dialog = QApplication::activeModalWidget();
      dialog->findChild<QDoubleSpinBox*>("distance")->setValue(distance);
      dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
    });
    action->trigger();
    undoStackOf(scene).hasElementCount(1).isCurrentlyAtElementNo(1);
    leftEdgeOf(m2).is(distance).fromRightEdgeOf(m1);
  }

  void testPuttingItemsAtIntervalHorizontally() {
    qreal interval = 20;
    action = LineUpAction::horizontal(scene);
    scene->selectAll();
    QTimer::singleShot(0, [&] {
      QWidget *dialog = QApplication::activeModalWidget();
      dialog->findChild<QRadioButton*>("equalInterval")->setChecked(true);
      dialog->findChild<QDoubleSpinBox*>("interval")->setValue(interval);
      dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
    });
    action->trigger();
    undoStackOf(scene).hasElementCount(1).isCurrentlyAtElementNo(1);
    centerXCoordinateOf(m2).is(interval).fromCenterXCoordinateOf(m1);
  }

  void testVerticalLiningUpWithDistance() {
    qreal distance = 10;
    action = LineUpAction::vertical(scene);
    scene->selectAll();
    QTimer::singleShot(0, [&] {
      QWidget *dialog = QApplication::activeModalWidget();
      dialog->findChild<QDoubleSpinBox*>("distance")->setValue(distance);
      dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
    });
    action->trigger();
    undoStackOf(scene).hasElementCount(1).isCurrentlyAtElementNo(1);
    topEdgeOf(m2).is(distance).fromBottomEdgeOf(m1);
  }

  void testPuttingItemsAtIntervalVertically() {
    qreal interval = 20;
    action = LineUpAction::vertical(scene);
    scene->selectAll();
    QTimer::singleShot(0, [&] {
      QWidget *dialog = QApplication::activeModalWidget();
      dialog->findChild<QRadioButton*>("equalInterval")->setChecked(true);
      dialog->findChild<QDoubleSpinBox*>("interval")->setValue(interval);
      dialog->findChild<QDialogButtonBox*>()->button(QDialogButtonBox::Ok)->click();
    });
    action->trigger();
    undoStackOf(scene).hasElementCount(1).isCurrentlyAtElementNo(1);
    centerYCoordinateOf(m2).is(interval).fromCenterYCoordinateOf(m1);
  }
};
