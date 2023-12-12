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
#include <atom.h>
#include <bond.h>
#include <graphicsitem.h>
#include "utilities.h"

using namespace Molsketch;

const QByteArray EMPTY_LIST_XML("<?xml version=\"1.0\" encoding=\"UTF-8\"?><molsketchItems/>\n");
const QByteArray ONE_ATOM_XML("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                               "<atom id=\"\" elementType=\"A\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"3,3\"/>\n");
const QByteArray TWO_ATOMS_XML("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                               "<molsketchItems>"
                               "<atom id=\"\" elementType=\"A\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"3,3\"/>"
                               "<atom id=\"\" elementType=\"B\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"4,5\"/>"
                               "</molsketchItems>\n"
                               );

class GraphicsItemUnitTest : public CxxTest::TestSuite {
  Atom *atomA, *atomB;
public:
  void setUp() {
    atomA = new Atom(QPointF(3,3), "A");
    atomB = new Atom(QPointF(4,5), "B");
  }

  void tearDown() {
    delete atomA;
    delete atomB;
  }

  void testSerializingEmptyListOfGraphicsItems() {
    QS_ASSERT_EQUALS(graphicsItem::serialize(QList<const graphicsItem*>()), EMPTY_LIST_XML);
  }

  void testSerializingListOfGraphicsItems() {
    QList<const graphicsItem*> items;
    items << atomA << atomB;
    QS_ASSERT_EQUALS(graphicsItem::serialize(items), TWO_ATOMS_XML);
  }

  void testDeserializingEmptyByteArray() {
    QS_ASSERT_EQUALS(graphicsItem::deserialize(QByteArray()), QList<graphicsItem*>());
  }

  void testDeserializingEmptyXml() {
    QS_ASSERT_EQUALS(graphicsItem::deserialize(EMPTY_LIST_XML), QList<graphicsItem*>());
  }

  void testDeserializingTwoAtoms() {
    auto deserialized = graphicsItem::deserialize(TWO_ATOMS_XML);
    TS_ASSERT_EQUALS(deserialized.size(), 2);
    QS_ASSERT_ON_POINTER(Atom, deserialized[0], element(), "A");
    QS_ASSERT_ON_POINTER(Atom, deserialized[1], element(), "B");
  }

  void testSerializingSingleAtom() {
    QS_ASSERT_EQUALS(graphicsItem::serialize(QList<const graphicsItem*>() << atomA), ONE_ATOM_XML);
  }

  void testDeserializingSingleAtom() {
    auto deserialized = graphicsItem::deserialize(ONE_ATOM_XML);
    TS_ASSERT_EQUALS(deserialized.size(), 1);
    QS_ASSERT_ON_POINTER(Atom, deserialized[0], element(), "A");
  }
};
