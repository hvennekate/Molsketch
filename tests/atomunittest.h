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

#include <bond.h>
#include <lonepair.h>
#include <molecule.h>
#include <sumformula.h>

#include <atom.h>
#include <radicalelectron.h>
#include "utilities.h"
#include "xmlassertion.h"

using namespace Molsketch;
using XmlAssert::assertThat;

const QString ATOM_XML_WITH_RADICAL("<atom"
                                    " id=\"\""
                                    " elementType=\"\""
                                    " userCharge=\"0\""
                                    " disableHydrogens=\"0\""
                                    " hydrogens=\"0\""
                                    " shapeType=\"0\""
                                    " hydrogenAlignment=\"0\""
                                    " colorR=\"0\""
                                    " colorG=\"0\""
                                    " colorB=\"0\""
                                    " scalingParameter=\"1\""
                                    " zLevel=\"3\""
                                    " coordinates=\"0,0\">"
                                    "<radicalElectron diameter=\"3\" colorR=\"0\" colorG=\"0\" colorB=\"0\">"
                                    "<bbLinker originAnchor=\"Top\" targetAnchor=\"Bottom\" xOffset=\"0\" yOffset=\"0\"/>"
                                    "</radicalElectron>"
                                    "</atom>");
const QString ATOM_XML_WITH_LONE_PAIR("<atom"
                                      " id=\"\""
                                      " elementType=\"\""
                                      " userCharge=\"0\""
                                      " disableHydrogens=\"0\""
                                      " hydrogens=\"0\""
                                      " shapeType=\"0\""
                                      " hydrogenAlignment=\"0\""
                                      " colorR=\"0\""
                                      " colorG=\"0\""
                                      " colorB=\"0\""
                                      " scalingParameter=\"1\""
                                      " zLevel=\"3\""
                                      " coordinates=\"0,0\">"
                                      "<lonePair angle=\"45\" length=\"10\" lineWidth=\"1.5\" colorR=\"0\" colorG=\"0\" colorB=\"0\">"
                                      "<bbLinker originAnchor=\"TopLeft\" targetAnchor=\"Center\" xOffset=\"0\" yOffset=\"0\"/>"
                                      "</lonePair>"
                                      "</atom>");
const QString ATOM_XML_WITH_LONE_PAIR_AND_RADICAL("<atom"
                                                  " id=\"\""
                                                  " elementType=\"\""
                                                  " userCharge=\"0\""
                                                  " disableHydrogens=\"0\""
                                                  " hydrogens=\"0\""
                                                  " shapeType=\"0\""
                                                  " hydrogenAlignment=\"0\""
                                                  " colorR=\"0\""
                                                  " colorG=\"0\""
                                                  " colorB=\"0\""
                                                  " scalingParameter=\"1\""
                                                  " zLevel=\"3\""
                                                  " coordinates=\"0,0\">"
                                                  "<lonePair angle=\"45\" length=\"10\" lineWidth=\"1.5\" colorR=\"0\" colorG=\"0\" colorB=\"0\">"
                                                  "<bbLinker originAnchor=\"TopLeft\" targetAnchor=\"Center\" xOffset=\"0\" yOffset=\"0\"/>"
                                                  "</lonePair>"
                                                  "<radicalElectron diameter=\"3\" colorR=\"0\" colorG=\"0\" colorB=\"0\">"
                                                  "<bbLinker originAnchor=\"Top\" targetAnchor=\"Bottom\" xOffset=\"0\" yOffset=\"0\"/>"
                                                  "</radicalElectron>"
                                                  "</atom>");
const QString LEGACY_ATOM_XML("<atom "
                              "id=\"testIndex\" "
                              "elementType=\"M\" "
                              "hydrogenCount=\"8\" "
                              "newmanDiameter=\"3.5\" "
                              "colorR=\"0\" "
                              "colorG=\"0\" "
                              "colorB=\"0\" "
                              "scalingParameter=\"1\" "
                              "zLevel=\"3\" "
                              "coordinates=\"0,0\"/>");
const QString ATOM_XML("<atom "
                       "id=\"testIndex\" "
                       "elementType=\"M\" "
                       "userCharge=\"-2\" "
                       "disableHydrogens=\"0\" "
                       "hydrogens=\"8\" "
                       "shapeType=\"0\" "
                       "newmanDiameter=\"3.5\" "
                       "hydrogenAlignment=\"0\" "
                       "colorR=\"0\" "
                       "colorG=\"0\" "
                       "colorB=\"0\" "
                       "scalingParameter=\"1\" "
                       "zLevel=\"3\" "
                       "coordinates=\"0,0\"/>"
);
const struct
{
  const QString ELEMENT = "M";
  const int CHARGE = -10;
  const double NEWMAN_DIAMETER = 3.5;
  const QString INDEX = "testIndex";
  const int HYDROGENS = 8;
} XML_DATA;
const char NEWMAN_XQUERY[] = "//*:atom/@newmanDiameter/data(.)";
const qreal RADICAL_DIAMETER = 3;
const qreal LONE_PAIR_ANGLE = 45;
const qreal LONE_PAIR_LINE_WIDTH = 1.5;
const qreal LONE_PAIR_LENGTH = 10;

class AtomUnitTest : public CxxTest::TestSuite {
  Atom *atom;
  RadicalElectron *radical;
  LonePair *lonePair;

  void assertXmlOutput(const QString expected) {
    QString output;
    QXmlStreamWriter writer(&output);
    atom->writeXml(writer);
    QS_ASSERT_EQUALS(output, expected);
  }

  void readXmlInput(const QString& input) {
    QXmlStreamReader reader(input); // TODO extract XML utilities
    reader.readNextStartElement();
    atom->readXml(reader);
  }

public:
  void setUp() {
    atom = new Atom();
    lonePair = new LonePair(LONE_PAIR_ANGLE, LONE_PAIR_LINE_WIDTH, LONE_PAIR_LENGTH, BoundingBoxLinker::atTopLeft());
    radical = new RadicalElectron(RADICAL_DIAMETER);
  }

  void tearDown() {
    delete lonePair;
    delete radical;
    for (QGraphicsItem* childItem : atom->childItems())
      delete childItem;
    delete atom;
  }

  void testXmlOutputWithRadicalElectron() {
    radical->setParentItem(atom);
    assertXmlOutput(ATOM_XML_WITH_RADICAL);
  }

  void testXmlInputWithRadicalElectron() {
    readXmlInput(ATOM_XML_WITH_RADICAL);
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_DIFFERS(dynamic_cast<RadicalElectron*>(atom->childItems().first()), nullptr);
    // TODO delete atom's children?
  }

  void testXmlOutputWithLonePair() {
    lonePair->setParentItem(atom);
    assertXmlOutput(ATOM_XML_WITH_LONE_PAIR);
  }

  void testXmlInputWithLonePair() {
    readXmlInput(ATOM_XML_WITH_LONE_PAIR);
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_DIFFERS(dynamic_cast<LonePair*>(atom->childItems().first()), nullptr);
  }

  void testXmlOutputWithLonePairAndRadicalElectron() {
    lonePair->setParentItem(atom);
    radical->setParentItem(atom);
    assertXmlOutput(ATOM_XML_WITH_LONE_PAIR_AND_RADICAL);

  }

  void testXmlInputWithLonePairAndRadicalElectron() {
    readXmlInput(ATOM_XML_WITH_LONE_PAIR_AND_RADICAL);
    TS_ASSERT_EQUALS(atom->childItems().size(), 2);
    TS_ASSERT_DIFFERS(dynamic_cast<LonePair*>(atom->childItems()[0]), nullptr);
    TS_ASSERT_DIFFERS(dynamic_cast<RadicalElectron*>(atom->childItems()[1]), nullptr);
  }

  void testAtomIsShownIfItHasChildren() {
    auto otherAtom = new Atom, thirdAtom = new Atom;
    auto firstBond = new Bond(atom, otherAtom), secondBond = new Bond(atom, thirdAtom); // need at least two bonds
    Molecule molecule(QSet<Atom*>{atom, otherAtom, thirdAtom}, QSet<Bond*>{firstBond, secondBond});

    atom->setElement("C");
    TS_ASSERT(!atom->isDrawn());

    lonePair->setParentItem(atom);
    TS_ASSERT(atom->isDrawn());

    molecule.delAtom(atom);
    delete firstBond;
    delete secondBond;
  }

  void testNormalAtomHasNoNewmanDiameter() {
    Atom atom(QPointF(), "C");
    assertThat(atom)->contains(NEWMAN_XQUERY)->never();
  }

  void testNewmanAtomHasNewmanDiameter() {
    Atom atom(QPointF(), "C");
    atom.setNewmanDiameter(5.5);
    TS_ASSERT_EQUALS(atom.getNewmanDiameter(), 5.5);
    assertThat(atom)->contains(NEWMAN_XQUERY)->exactlyOnceWithContent("5.5");
  }

  void testDisabledNewmanAtomHasNoNewmanDiameter() {
    Atom atom(QPointF(), "C");
    atom.setNewmanDiameter(5.5);
    TS_ASSERT_EQUALS(atom.getNewmanDiameter(), 5.5);
    atom.disableNewman();
    assertThat(atom)->contains(NEWMAN_XQUERY)->never();
  }

  void testAtomWithNewmanDiameterCanBeRead() {
    Atom atom;
    TS_ASSERT_EQUALS(atom.getNewmanDiameter(), 0);
    QXmlStreamReader in("<atom elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"0,0\" newmanDiameter=\"3.5\"/>");
    in.readNextStartElement();
    atom.readXml(in);
    TS_ASSERT_EQUALS(atom.getNewmanDiameter(), 3.5);
  }

  void testAtomWithoutNewmanDiameterCanBeRead() {
    Atom atom;
    atom.setNewmanDiameter(5.5);
    TS_ASSERT_EQUALS(atom.getNewmanDiameter(), 5.5);
    QXmlStreamReader in("<atom elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"0,0\"/>");
    in.readNextStartElement();
    atom.readXml(in);
    TS_ASSERT_EQUALS(atom.getNewmanDiameter(), 0);
  }

  void testBoundingRectForCarbonNewmanAtom() {
    Atom a1, a2;
    Bond b1(&a1, atom), b2(atom, &a2);
    atom->setElement("C");
    atom->setCharge(0);
    atom->setNewmanDiameter(6);
    TS_ASSERT(atom->isDrawn());
    QS_ASSERT_EQUALS(atom->boundingRect(), QRectF(-3, -3, 6, 6));
  }

  void testBondDrawingStart() {
    atom->setElement("A");
    auto otherAtomPosition = QPointF(10,15);
    Atom otherAtom(otherAtomPosition); // TODO more test cases

    auto atomBounds = atom->boundingRect();
    auto rightEdge = QLineF(atomBounds.topRight(), atomBounds.bottomRight());
    QPointF intersectionOfRightEdgeAndConnectingLine;
    QS_ASSERT_EQUALS(rightEdge.intersect(QLineF(QPointF(0,0), otherAtomPosition), &intersectionOfRightEdgeAndConnectingLine),
                     QLineF::BoundedIntersection);

    QS_ASSERT_EQUALS(atom->bondDrawingStart(&otherAtom, 0), intersectionOfRightEdgeAndConnectingLine);
  }

  void testSumFormula() {
    atom->setElement("C3H5");
    auto c3h5 = SumFormula{{"C", 3}, {"H", 5}};
    QS_ASSERT_EQUALS(atom->sumFormula(), c3h5);

    atom->setElement("C");
    atom->setNumImplicitHydrogens(0);
    atom->setCharge(0);
    QS_ASSERT_EQUALS(atom->sumFormula(), SumFormula{"C"});

    atom->setNumImplicitHydrogens(2);
    atom->setCharge(0);
    SumFormula ch2{{"C"}, {"H", 2}};
    QS_ASSERT_EQUALS(atom->sumFormula(), ch2);

    atom->setCharge(-3);
    SumFormula ch2_3minus{{"C"}, {"H", 2, -3}};
    QS_ASSERT_EQUALS(atom->sumFormula(), ch2_3minus);
  }

  void testWritingAllAttributes() {
    atom->setElement(XML_DATA.ELEMENT);
    atom->setNewmanDiameter(XML_DATA.NEWMAN_DIAMETER);
    atom->setIndex(XML_DATA.INDEX);
    atom->setNumImplicitHydrogens(XML_DATA.HYDROGENS);
    atom->setCharge(XML_DATA.CHARGE);
    assertXmlOutput(ATOM_XML);
  }

  void testReadingAllAttributes() {
    readXmlInput(ATOM_XML);
    QS_ASSERT_EQUALS(atom->element(), XML_DATA.ELEMENT);
    TS_ASSERT_EQUALS(atom->charge(), XML_DATA.CHARGE);
    TS_ASSERT_EQUALS(atom->getNewmanDiameter(), XML_DATA.NEWMAN_DIAMETER);
    QS_ASSERT_EQUALS(atom->index(), XML_DATA.INDEX);
    TS_ASSERT_EQUALS(atom->numImplicitHydrogens(), XML_DATA.HYDROGENS);
  }

  void testReadingLegacyAtom() {
    QXmlStreamReader reader(LEGACY_ATOM_XML);
    reader.readNextStartElement();
    auto legacyAtom = new LegacyAtom;
    legacyAtom->readXml(reader);
    QS_ASSERT_EQUALS(legacyAtom->element(), XML_DATA.ELEMENT);
    TS_ASSERT_EQUALS(legacyAtom->charge(), 0);
    TS_ASSERT_EQUALS(legacyAtom->getNewmanDiameter(), XML_DATA.NEWMAN_DIAMETER);
    QS_ASSERT_EQUALS(legacyAtom->index(), XML_DATA.INDEX);
    TS_ASSERT_EQUALS(legacyAtom->numImplicitHydrogens(), 0);
    legacyAtom->afterMoleculeReadFinalization();
    TS_ASSERT_EQUALS(legacyAtom->numImplicitHydrogens(), XML_DATA.HYDROGENS);
    delete legacyAtom;
  }
};
