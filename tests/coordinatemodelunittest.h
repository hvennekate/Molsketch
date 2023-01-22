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

#include "utilities.h"
#include <coordinatemodel.h>

using namespace Molsketch;

class CoordinateModelUnitTest : public CxxTest::TestSuite {
  CoordinateModel *model;

  QMap<QPair<Qt::Orientation, Qt::ItemDataRole>, QVariant> orientationRoleMap() {
    using namespace Qt;
    QVector<Orientation> orientations;
    orientations << Horizontal << Vertical;

    QVector<ItemDataRole> itemDataRoles;
    itemDataRoles <<
                     DisplayRole <<
                     DecorationRole <<
                     EditRole <<
                     ToolTipRole <<
                     StatusTipRole <<
                     WhatsThisRole <<
                     FontRole <<
                     TextAlignmentRole <<
                     BackgroundRole <<
                     ForegroundRole <<
                     CheckStateRole <<
                     AccessibleTextRole <<
                     AccessibleDescriptionRole <<
                     SizeHintRole <<
                     InitialSortOrderRole <<
                     DisplayPropertyRole <<
                     DecorationPropertyRole <<
                     ToolTipPropertyRole <<
                     StatusTipPropertyRole <<
                     WhatsThisPropertyRole <<
                     UserRole;

    QMap<QPair<Orientation, ItemDataRole>, QVariant> result;
    for (Orientation orientation : orientations)
      for (ItemDataRole itemDataRole : itemDataRoles)
        result[QPair<Orientation, ItemDataRole>(orientation, itemDataRole)] = QVariant();

    return result;
  }

public:
  void setUp() {
    model = new CoordinateModel;
  }

  void tearDown(){
    delete model;
  }

  void testColumnCount() {
    TS_ASSERT_EQUALS(model->columnCount(QModelIndex()), 2);
  }

  void testRowCount() {
    const int coordinateCount = 5;
    model->setCoordinates(QPolygon(coordinateCount));
    TS_ASSERT_EQUALS(model->rowCount(QModelIndex()), coordinateCount);
  }


  void testHeaderData() {
    auto expectedResults = orientationRoleMap();
    for (int section = -1; section < 3; ++section) {
      expectedResults[QPair<Qt::Orientation, Qt::ItemDataRole>(Qt::Horizontal, Qt::DisplayRole)] = (section == 0 ? QVariant("x") : (section == 1 ? QVariant("y") : QVariant()));
      for (auto key : expectedResults.keys())
        QS_ASSERT_EQUALS(model->headerData(section, key.first, key.second), expectedResults[key]);
    }
  }

  void setSampleData() {
    model->setCoordinates(QPolygonF() << QPointF(3.5, 4.3) << QPointF(2.7, 1.9));
  }

  void testData() {
    setSampleData();
    TS_ASSERT_EQUALS(model->rowCount(QModelIndex()), 2);
    TS_ASSERT_EQUALS(model->columnCount(QModelIndex()), 2);
    for (int role : QVector<int>{Qt::DisplayRole, Qt::EditRole}) {
      TSM_ASSERT_EQUALS(role, model->data(model->index(0,0), role).toDouble(), 3.5);
      TSM_ASSERT_EQUALS(role, model->data(model->index(0,1), role).toDouble(), 4.3);
      TSM_ASSERT_EQUALS(role, model->data(model->index(1,0), role).toDouble(), 2.7);
      TSM_ASSERT_EQUALS(role, model->data(model->index(1,1), role).toDouble(), 1.9);
    }
  }

  void testSetData() {
    setSampleData();
    model->setData(model->index(0,1), 7.5, Qt::EditRole);
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF(3.5, 7.5) << QPointF(2.7, 1.9));
  }

  void testFlags() {
    setSampleData();
    for (int i = 0 ; i < 2 ; ++i)
      for (int j = 0 ; j < 2 ; ++j)
        TS_ASSERT_EQUALS(model->flags(model->index(i,j)), Qt::ItemNeverHasChildren | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
  }

  void testInsertRowsBeginning() {
    setSampleData();
    TS_ASSERT(model->insertRows(0, 2, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF(3.5, 4.3) << QPointF(3.5, 4.3) << QPointF(3.5, 4.3) << QPointF(2.7, 1.9));
  }
  void testInsertRowsMiddle() {
    setSampleData();
    TS_ASSERT(model->insertRows(1, 2, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF(3.5, 4.3) << QPointF(2.7, 1.9) << QPointF(2.7, 1.9) << QPointF(2.7, 1.9));
  }

  void testInsertRowsEnd() {
    setSampleData();
    TS_ASSERT(model->insertRows(2, 2, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF(3.5, 4.3) << QPointF(2.7, 1.9) << QPointF(2.7, 1.9) << QPointF(2.7, 1.9));
  }

  void testInsertRowsEmpty() {
    TS_ASSERT(model->insertRows(0, 2, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF() << QPointF());
  }

  void testInsertRowsFailsForNegative() {
    TS_ASSERT(!model->insertRows(-1, 2, QModelIndex()));
    setSampleData();
    TS_ASSERT(!model->insertRows(-1, 2, QModelIndex()));
  }

  void testInsertRowsFailsForLarger() {
    TS_ASSERT(!model->insertRows(1, 2, QModelIndex()));
    setSampleData();
    TS_ASSERT(!model->insertRows(3, 2, QModelIndex()));
  }

  void testRemoveRows() {
    TS_ASSERT(!model->removeRows(-1, 1, QModelIndex()));
    TS_ASSERT(!model->removeRows( 0, 1, QModelIndex()));
    TS_ASSERT(!model->removeRows( 1, 1, QModelIndex()));

    setSampleData();
    TS_ASSERT(!model->removeRows(-1, 1, QModelIndex()));
    TS_ASSERT(!model->removeRows( 2, 1, QModelIndex()));
    TS_ASSERT(model->removeRows(0, 1, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF(2.7, 1.9));

    setSampleData();
    TS_ASSERT(model->removeRows(1, 1, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF() << QPointF(3.5, 4.3));

    setSampleData();
    TS_ASSERT(model->removeRows(0, 2, QModelIndex()));
    QS_ASSERT_EQUALS(model->getCoordinates(), QPolygonF());
  }

  void testInsertColumnsFails() {
    TS_ASSERT(!model->insertColumns(-1, 1, QModelIndex()));
    TS_ASSERT(!model->insertColumns( 0, 1, QModelIndex()));
    TS_ASSERT(!model->insertColumns( 1, 1, QModelIndex()));
    setSampleData();
    TS_ASSERT(!model->insertColumns(-1, 1, QModelIndex()));
    TS_ASSERT(!model->insertColumns(0, 1, QModelIndex()));
    TS_ASSERT(!model->insertColumns(2, 1, QModelIndex()));
    TS_ASSERT(!model->insertColumns(3, 1, QModelIndex()));
  }

  void testRemoveColumnsFails() {
    TS_ASSERT(!model->removeColumns(-1, 1, QModelIndex()));
    TS_ASSERT(!model->removeColumns(0, 1, QModelIndex()));
    TS_ASSERT(!model->removeColumns(1, 1, QModelIndex()));
    setSampleData();
    TS_ASSERT(!model->removeColumns(-1, 1, QModelIndex()));
    TS_ASSERT(!model->removeColumns(0, 1, QModelIndex()));
    TS_ASSERT(!model->removeColumns(2, 1, QModelIndex()));
    TS_ASSERT(!model->removeColumns(3, 1, QModelIndex()));
  }
};
