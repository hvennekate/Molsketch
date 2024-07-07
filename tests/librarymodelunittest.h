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

#include <functional>
#include <moleculemodelitem.h>
#include "utilities.h"
#include "mocks.h"
#include <cxxtest/TestSuite.h>
#include <QMimeData>
#include <QModelIndex>
#include <librarymodel.h>
#include <constants.h>
#include <molecule.h>
#include <QList>
#include "signalcounter.h"
#include "noargsignalcounter.h"

using namespace Molsketch;

class LibraryModelUnitTest : public CxxTest::TestSuite {
  CLASS_FOR_TESTING(Molecule)
  CLASS_FOR_TESTING(LibraryModel)
  CLASS_FOR_TESTING_WITH_FUNCTIONS(MoleculeModelItem,
      Molecule *molecule;
    public:
      explicit MoleculeModelItemForTesting(Molecule* molecule = nullptr) : molecule(molecule){}
      Molecule* produceMolecule() const { return molecule; })

  LibraryModelForTesting* model;

public:
void setUp() {
  model = new LibraryModelForTesting;
  MoleculeForTesting::instanceCounter = 0;
}

void tearDown() {
  delete model;
  model = 0;
  TSM_ASSERT_EQUALS("Molecules not cleaned up!", MoleculeForTesting::instanceCounter, 0);
}

void testForEmptyCannotFetchMore() {
  TS_ASSERT(!model->canFetchMore(QModelIndex()));
}

void testRowCount() {
  model->setMolecules(QList<MoleculeModelItem*>()
                      << new MoleculeModelItemForTesting
                      << new MoleculeModelItemForTesting
                      << new MoleculeModelItemForTesting);
  TS_ASSERT_EQUALS(model->rowCount(), 0);
  TS_ASSERT(model->canFetchMore(QModelIndex()));
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(model->rowCount(), 3);
  TS_ASSERT(!model->canFetchMore(QModelIndex()));
}

void testRowCountSameMoleculeMultipleTimes() {
  MoleculeModelItem *m = new MoleculeModelItemForTesting;
  model->setMolecules(QList<MoleculeModelItem*>() << m << m << m);
  TS_ASSERT_EQUALS(model->rowCount(), 0);
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(model->rowCount(), 3);
  model->setMolecules(QList<MoleculeModelItem*>() << new MoleculeModelItemForTesting);
  TS_ASSERT_EQUALS(model->rowCount(), 0);
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(model->rowCount(), 1);
}

void testRowCountByAddingIndividually() {
  model->addMolecule(new MoleculeModelItemForTesting);
  model->addMolecule(new MoleculeModelItemForTesting);
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(model->rowCount(), 2);
}

void testMoleculesAreAddedTwice() {
  MoleculeModelItem *m = new MoleculeModelItemForTesting;
  model->addMolecule(m);
  model->addMolecule(m);
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(model->rowCount(), 2);
}

void testSettingMoleculesTriggersModelReset() {
  NoArgSignalCounter beginResetCounter, endResetCounter;
  beginResetCounter.callback = [&] { TS_ASSERT_EQUALS(beginResetCounter.count, endResetCounter.count +1); };
  endResetCounter.callback = [&] { TS_ASSERT_EQUALS(beginResetCounter.count, endResetCounter.count); };
  QObject::connect(model, SIGNAL(modelAboutToBeReset()), &beginResetCounter, SLOT(record()));
  QObject::connect(model, SIGNAL(modelReset()), &endResetCounter, SLOT(record()));
  model->setMolecules(QList<MoleculeModelItem*>());
  TS_ASSERT_EQUALS(beginResetCounter.count, 1);
  TS_ASSERT_EQUALS(endResetCounter.count, 1);
}

void checkInsertionForRow(std::tuple<QModelIndex, int, int> input, int expectedRow) {
  QModelIndex index;
  int startRow, endRow;
  std::tie(index, startRow, endRow) = input;
  TS_ASSERT(!index.isValid());
  TS_ASSERT_EQUALS(startRow, expectedRow);
  TS_ASSERT_EQUALS(endRow, expectedRow);
}

void testAddingMoleculeTriggersSignals() {
  ModelIndexSignalCounter beforeInsert, afterInsert;
  beforeInsert.callback = [&] (QModelIndex index, int begin, int end) {TS_ASSERT_EQUALS(beforeInsert.count() - 1,
                                                                                         afterInsert.count()) };
  afterInsert.callback = [&] (QModelIndex index, int begin, int end) { TS_ASSERT_EQUALS(beforeInsert.count(),
                                                                                        afterInsert.count())}; // TODO check ints
  QObject::connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
                   &beforeInsert, SLOT(record(QModelIndex,int,int)));
  QObject::connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                   &afterInsert, SLOT(record(QModelIndex,int,int)));

  model->addMolecule(new MoleculeModelItemForTesting(new Molecule()));
  TS_ASSERT_EQUALS(beforeInsert.count(), 0);
  TS_ASSERT_EQUALS(afterInsert.count(), 0);
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(beforeInsert.count(), 1);
  TS_ASSERT_EQUALS(afterInsert.count(), 1);
  checkInsertionForRow(beforeInsert.getLatestPayload(), 0);
  checkInsertionForRow(afterInsert.getLatestPayload(), 0);

  model->addMolecule(new MoleculeModelItemForTesting(new Molecule()));
  model->fetchMore(QModelIndex());
  TS_ASSERT_EQUALS(beforeInsert.count(), 2);
  TS_ASSERT_EQUALS(beforeInsert.count(), 2);
  checkInsertionForRow(beforeInsert.getLatestPayload(), 1);
  checkInsertionForRow(afterInsert.getLatestPayload(), 1);
}

void testDeletingOldMolecules() {
  model->setMolecules(QList<MoleculeModelItem*>() << new MoleculeModelItemForTesting());
  model->setMolecules(QList<MoleculeModelItem*>() << new MoleculeModelItemForTesting());
  TS_ASSERT_EQUALS(MoleculeModelItemForTesting::instanceCounter, 1);
}

void testMimeData() {
  Molecule *m = new MoleculeForTesting;
  QByteArray expectedXml;
  QXmlStreamWriter xmlWriter(&expectedXml);
  xmlWriter.writeStartDocument();
  xmlWriter << *m;
  xmlWriter.writeEndDocument();
  model->addMolecule(new MoleculeModelItemForTesting(m));
  model->fetchMore(QModelIndex());
  QByteArray obtainedData = model->mimeData(QModelIndexList() << model->index(0,0))->data(moleculeMimeType);
  QS_ASSERT_EQUALS(obtainedData, expectedXml);
}

void testDataForDecorationRole() {
  Molecule *m = new MoleculeForTesting;
  Atom *a1 = new Atom(QPointF(), "C");
  Atom *a2 = new Atom(QPointF(10, 10), "N");
  m->addAtom(a1);
  m->addAtom(a2);
  m->addBond(new Bond(a1, a2));

  model->addMolecule(new MoleculeModelItemForTesting(m));
  model->fetchMore(QModelIndex());
  auto actualIcon = model->data(model->index(0), Qt::DecorationRole).value<QIcon>();
  TS_ASSERT(!actualIcon.isNull()); // TODO compare against expected icon
}

void testDataForDisplayRole() {
  Molecule *m = new MoleculeForTesting;
  m->setName("testname");
  model->addMolecule(new MoleculeModelItemForTesting(m));
  model->fetchMore(QModelIndex());
  QS_ASSERT_EQUALS(model->data(model->index(0)).toString(), "testname");
}

void testEmptyReturns() {
  TS_ASSERT_EQUALS(model->rowCount(), 0);
  TS_ASSERT(model->data(QModelIndex()).isNull());
  QMimeData *mimeData = model->mimeData(QModelIndexList());
  TS_ASSERT(mimeData->hasFormat(moleculeMimeType));
  QS_ASSERT_EQUALS(mimeData->data(moleculeMimeType), "<?xml version=\"1.0\" encoding=\"UTF-8\"?><molsketchItems/>\n");
  delete mimeData;
}

// TODO: thread safety
};
