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

#include "noargsignalcounter.h"

#include <cxxtest/TestSuite.h>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSet>
#include <actions/itemtypeselectionaction.h>
#include "utilities.h"

class TestGraphicsItem : public QGraphicsItem {
public:
  static int itemCount;
  TestGraphicsItem() {
    ++itemCount;
    setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
  }
  virtual ~TestGraphicsItem() { --itemCount; }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){}
  QRectF boundingRect() const { return QRectF(); }
};

int TestGraphicsItem::itemCount = 0;

#define TESTITEM(NAME, TYPE_OFFSET) \
  class NAME : public TestGraphicsItem { \
  public: \
  enum { Type = UserType + TYPE_OFFSET }; \
  int type() const { return Type; } \
  };

TESTITEM(UndesiredItem, 1)
TESTITEM(DesiredItem, 2)
TESTITEM(OtherDesiredItem, 3)

using namespace Molsketch;
class ItemTypeSelectionActionUnitTest : public CxxTest::TestSuite {
  QList<QGraphicsItem*> items;
  int originalItemCount;
public:
  void setUp() {
    originalItemCount = TestGraphicsItem::itemCount;
  }

  void tearDown() {
    for (QGraphicsItem *item : items) delete item;
    items.clear();
    TSM_ASSERT_EQUALS("Not all items were deleted!", TestGraphicsItem::itemCount, originalItemCount);
  }

  void testOnlyDesiredTypeGetsSelected() {
    QGraphicsItem* desiredItem = new DesiredItem;
    items << nullptr;
    items << new UndesiredItem;
    items << desiredItem;
    QList<QGraphicsItem*> result = ItemTypeSelectionAction::itemsByType(items, QList<int>() << DesiredItem::Type);
    TS_ASSERT_EQUALS(result.size(), 1);
    TS_ASSERT_EQUALS(result.first(), desiredItem); // TODO proper list assertions/build proper assertion framework
  }

  void testWithAllItemsOfSelectedType() {
    QGraphicsItem *desiredItem = new DesiredItem, *secondDesiredItem = new DesiredItem;
    items << desiredItem;
    items << secondDesiredItem;
    QList<QGraphicsItem*> result = ItemTypeSelectionAction::itemsByType(items, QList<int>() << DesiredItem::Type);
    TS_ASSERT_EQUALS(result.size(), 2);
    TS_ASSERT(result.contains(desiredItem));
    TS_ASSERT(result.contains(secondDesiredItem));
  }

  void testWithNoItems() {
    TS_ASSERT(ItemTypeSelectionAction::itemsByType(items, QList<int>()).isEmpty());
  }

  void testMultipleSelectedTypes() {
    QGraphicsItem *desiredItem = new DesiredItem;
    QGraphicsItem *otherDesiredItem = new OtherDesiredItem;
    items << desiredItem;
    items << otherDesiredItem;
    QList<QGraphicsItem*> result =
        ItemTypeSelectionAction::itemsByType(items,
                                             QList<int>()
                                             << DesiredItem::Type
                                             << OtherDesiredItem::Type);
    TS_ASSERT_EQUALS(result.size(), 2);
    TS_ASSERT(result.contains(desiredItem));
    TS_ASSERT(result.contains(otherDesiredItem));
  }

  void testRecursiveSelectionOfChildItems() {
    QGraphicsItem *undesiredItem = new UndesiredItem;
    QGraphicsItem *desiredItem = new DesiredItem;
    desiredItem->setParentItem(undesiredItem);
    items << undesiredItem;
    QList<QGraphicsItem*> result = ItemTypeSelectionAction::itemsByType(items, QList<int>() << DesiredItem::Type);
    TS_ASSERT_EQUALS(result.size(), 1);
    TS_ASSERT(result.contains(desiredItem));
  }

  void testChildrenSkippedIfParentAlreadySelected() {
    QGraphicsItem *desiredItem = new DesiredItem;
    QGraphicsItem *childItem = new DesiredItem;
    childItem->setParentItem(desiredItem);
    items << desiredItem;
    QList<QGraphicsItem*> result = ItemTypeSelectionAction::itemsByType(items, QList<int>() << DesiredItem::Type);
    TS_ASSERT_EQUALS(result.size(), 1);
    TS_ASSERT(result.contains(desiredItem));
  }

  void testSelectionOfItemsInScene() {
    TestGraphicsItem *testItem = new TestGraphicsItem,
        *secondTestItem = new TestGraphicsItem;
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addItem(testItem);
    scene->addItem(secondTestItem);
    items << testItem << secondTestItem;
    NoArgSignalCounter signalCounter;
    QObject::connect(scene, SIGNAL(selectionChanged()), &signalCounter, SLOT(record()));

    TS_ASSERT(scene->selectedItems().isEmpty());
    ItemTypeSelectionAction::selectItems(scene, items);
    TS_ASSERT_EQUALS(scene->selectedItems().size(), items.size());
    TS_ASSERT_EQUALS(toSet(scene->selectedItems()), toSet(items));
    TS_ASSERT_EQUALS(signalCounter.count, items.count());
    TS_ASSERT(testItem->isSelected());
    TS_ASSERT(secondTestItem->isSelected());
    delete scene;
    items.clear();
  }

  void testSelectionClearedForEmptyListOfItems() {
    TestGraphicsItem *testItem = new TestGraphicsItem;
    testItem->setSelected(true);
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addItem(testItem);
    NoArgSignalCounter signalCounter;
    QObject::connect(scene, SIGNAL(selectionChanged()), &signalCounter, SLOT(record()));

    TS_ASSERT_EQUALS(scene->selectedItems().size(), 1);
    ItemTypeSelectionAction::selectItems(scene, items);
    TS_ASSERT(scene->selectedItems().isEmpty());
    TS_ASSERT_EQUALS(signalCounter.count, 1);
    TS_ASSERT(!testItem->isSelected());
    delete scene;
  }

  void testSelectionOfNullItem() {
    QGraphicsScene *scene = new QGraphicsScene;
    NoArgSignalCounter signalCounter;
    items << nullptr;
    QObject::connect(scene, SIGNAL(selectionChanged()), &signalCounter, SLOT(record()));

    ItemTypeSelectionAction::selectItems(scene, items);
    delete scene;
  }
};
