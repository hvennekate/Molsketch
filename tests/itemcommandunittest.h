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
#include <molscene.h>
#include <commands.h>

using namespace Molsketch;

CLASS_FOR_TESTING_WITH_FUNCTIONS(QGraphicsItem, \
                                 QRectF boundingRect() const override {return QRectF();} \
                                 void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override { Q_UNUSED(painter) Q_UNUSED(option) Q_UNUSED(widget) })

class ItemCommandUnitTest : public CxxTest::TestSuite {
  MolScene *scene;
  QGraphicsItem *item;

  void addItemToScene() {
    Commands::ItemAction::addItemToScene(item, scene);
  }

  void deleteItemFromScene() {
    Commands::ItemAction::removeItemFromScene(item);
  }

  void undo() {
    scene->stack()->undo();
  }
public:
  void setUp() {
    scene = new MolScene;
    item = new QGraphicsItemForTesting;
    TS_ASSERT_EQUALS(QGraphicsItemForTesting::instanceCounter, 1);
  }

  void tearDown() {
    delete scene;
    TS_ASSERT_EQUALS(QGraphicsItemForTesting::instanceCounter, 0);
  }

  void testAddingAndDeletingItem() {
    addItemToScene();
    TS_ASSERT(scene->stack()->canUndo());
    TS_ASSERT(scene->items().contains(item));
    deleteItemFromScene();
    TS_ASSERT(!scene->items().contains(item));
    TS_ASSERT_EQUALS(scene->stack()->count(), 2);
  }

  void testCrashAfterDeletingItem() {
    addItemToScene();
    deleteItemFromScene();
    scene->clear();
  }

  void testItemDeletedAfterAdding() {
    addItemToScene();
  }

  void testItemDeletedAfterUndoingAdding() {
    addItemToScene();
    undo();
    TS_ASSERT(!scene->items().contains(item));
  }

  void testItemDeletedAfterUndoingDeleteAction() {
    addItemToScene();
    deleteItemFromScene();
    undo();
    TS_ASSERT(scene->items().contains(item));
  }

  void testAddingItemToSceneThatIsAlreadyOnScene() {
    scene->addItem(item);
    addItemToScene();
    TS_ASSERT(scene->items().contains(item));
    TS_ASSERT(scene->stack()->canUndo());
  }

  void testAddingItemFromOtherScene() {
    MolScene otherScene;
    otherScene.addItem(item);
    addItemToScene();

    TS_ASSERT(!otherScene.items().contains(item));
    TS_ASSERT(scene->items().contains(item));
    TS_ASSERT(scene->stack()->canUndo());
  }

  void testDeletingItemFromSceneThatIsNotOnScene() {
    deleteItemFromScene();
    TS_ASSERT(!scene->items().contains(item));
    TS_ASSERT(!scene->stack()->canUndo());
  }

  void testDeletingAndUndoingRestoresParent() {
    auto parentItem = new QGraphicsItemForTesting;
    scene->addItem(parentItem);
    item->setParentItem(parentItem);

    deleteItemFromScene();

    TS_ASSERT(!scene->items().contains(item));
    TS_ASSERT(scene->items().contains(parentItem));

    undo();

    TS_ASSERT(scene->items().contains(item));
    TS_ASSERT(scene->items().contains(parentItem));
    TS_ASSERT_EQUALS(item->parentItem(), parentItem);
  }
};
