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

#include <cxxtest/TestSuite.h>
#include "utilities.h"
#include <molscene.h>
#include <molview.h>
#include <QByteArray>
#include <QXmlStreamWriter>
#include <molecule.h>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QCoreApplication>
#include <QDebug>
#include <algorithm>
#include <QGraphicsItem>

using namespace Molsketch;

class MoleculeLibraryAcceptanceTest : public CxxTest::TestSuite {
  QMimeData* prepareMimeData() {
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MolScene::mimeType(), serializeMolecule(sampleMolecule()));
    return mimeData;
  }

  QByteArray serializeMolecule(const Molecule& molecule) {
    QByteArray data;
    QXmlStreamWriter xmlWriter(&data);
    xmlWriter.writeStartDocument();
    xmlWriter << molecule;
    xmlWriter.writeEndDocument();
    qDebug() << data;
    return data;
  }

  Molecule sampleMolecule() {
    Atom *atomA = new Atom(QPointF(5,-5), "C");
    Atom *atomB = new Atom(QPointF(45,35), "C");
    return Molecule(QSet<Atom*>() << atomA << atomB,
                    QSet<Bond*>() << new Bond(atomA, atomB));
  }

  void prepareAndSendEvent(QEvent::Type type) {
    QGraphicsSceneDragDropEvent *event = new QGraphicsSceneDragDropEvent(type);
    event->setMimeData(prepareMimeData());
    event->setProposedAction(Qt::CopyAction);
    event->setScenePos(QPointF(10,5));
    TS_ASSERT(QCoreApplication::sendEvent(scene, event));
  }

  void assertMoleculeAndAtomCoordinates(MolScene* scene) {
    assertMoleculePosition(scene);
    assertAtomPositions(scene);
  }

  void assertMoleculePosition(MolScene* scene)
  {
    Molecule* moleculeItem = 0;
    for (QGraphicsItem* item : scene->items())
      if(dynamic_cast<Molecule*>(item))
        moleculeItem = dynamic_cast<Molecule*>(item);
    QS_ASSERT_ON_POINTER(Molecule, moleculeItem, pos(), QPointF(0,0));
  }

  void assertAtomPositions(MolScene* scene)
  {
    QS_ASSERT_ON_POINTER(Atom, scene->itemAt(-10, -15, QTransform()), coordinates(), QPolygonF() << QPointF(-10, -15));
    QS_ASSERT_ON_POINTER(Atom, scene->itemAt(-10, -15, QTransform()), pos(), QPointF(-10, -15));
    QS_ASSERT_ON_POINTER(Atom, scene->itemAt( 30,  25, QTransform()), coordinates(), QPolygonF() << QPointF(30, 25));
    QS_ASSERT_ON_POINTER(Atom, scene->itemAt( 30,  25, QTransform()), pos(), QPointF(30, 25));
  }

  MolScene *scene;
public:
  void setUp() {
    scene = new MolScene;
  }

  void tearDown() {
    delete scene;
  }

  void testBondsCanBeAddedAfterInsertingMolecule() {
    prepareAndSendEvent(QEvent::GraphicsSceneDragEnter);
    assertMoleculeAndAtomCoordinates(scene);

    prepareAndSendEvent(QEvent::GraphicsSceneDrop);
    assertMoleculeAndAtomCoordinates(scene);
  }


  // TODO check for multiple molecules
};
