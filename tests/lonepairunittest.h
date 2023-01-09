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
#include <molscene.h>
#include <lonepair.h>
#include "utilities.h"

using namespace Molsketch;

const qreal DELTA = 3e-4;
const qreal LENGTH = 10;
const qreal ANGLE = 45;
const qreal LINE_WIDTH = 1.5;
const BoundingBoxLinker ANCHOR = BoundingBoxLinker::atTopLeft();
const LonePair SAMPLE_LONE_PAIR(23.5, 1.5, 5.5, BoundingBoxLinker::atBottomLeft(), Qt::blue);
const QString LONE_PAIR_XML("<lonePair angle=\"23.5\" length=\"5.5\" lineWidth=\"1.5\" colorR=\"0\" colorG=\"0\" colorB=\"255\">"
                            "<bbLinker originAnchor=\"BottomLeft\" targetAnchor=\"Center\" xOffset=\"0\" yOffset=\"0\"/>"
                            "</lonePair>");

class LonePairUnitTest : public CxxTest::TestSuite {
  MolScene *scene;
  Atom *atom;

  QLineF obtainLineFromSvg() {
    LonePair *lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH, ANCHOR);
    lp->setParentItem(atom);
    QXmlStreamReader reader(scene->toSvg());
    TS_ASSERT(findNextElement(reader, "polyline"));
    QPolygonF points = getPointsFromXml(reader);
    QSM_ASSERT_EQUALS(reader.attributes().value("points").toString(), points.size(), 2);
    return QLineF(points[0], points[1]);
  }

  QXmlStreamAttributes obtainLineAttributesFromSvg() {
    LonePair *lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH, BoundingBoxLinker::atTop(), Qt::red);
    lp->setParentItem(atom);
    QXmlStreamReader reader(scene->toSvg());
    return getAttributesOfParentElement(reader, "polyline");
  }

public:
  void setUp() {
    scene = new MolScene;
    atom = new Atom(QPointF(0,0), "C");
    scene->addItem(atom);
  }

  void tearDown() {
    delete scene;
  }

  void testNothingDrawnWithoutParent() {
    scene->addItem(new LonePair(ANGLE, LINE_WIDTH, LENGTH));
    TS_ASSERT_EQUALS(xmlElementCount(scene->toSvg(), "polyline"), 0);
  }

  void testLonePairDrawnIfParentIsAtom() {
    LonePair *lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH);
    lp->setParentItem(atom);
    TS_ASSERT_EQUALS(xmlElementCount(scene->toSvg(), "polyline"), 1);
  }

  void testLengthAttribute() {
    TS_ASSERT_DELTA(obtainLineFromSvg().length(), LENGTH, DELTA);
  }

  void testAngleAttribute() {
    TS_ASSERT_DELTA(obtainLineFromSvg().angle(), ANGLE, DELTA);
  }

  void testLinewidthAttribute() {
    QS_ASSERT_EQUALS(obtainLineAttributesFromSvg().value("stroke-width").toDouble(), LINE_WIDTH);
  }

  void testColorAttribute() {
    QS_ASSERT_EQUALS(QColor(obtainLineAttributesFromSvg().value("stroke").toString().toStdString().data()),
                     QColor(Qt::red));
  }

  void testPositioning() {
    QLineF expected = QLineF::fromPolar(LENGTH, ANGLE);
    expected.translate(-expected.p2()/2);
    expected.translate(atom->boundingRect().topLeft());
    TS_ASSERT_DELTA(obtainLineFromSvg().p1().x(), expected.p1().x(), DELTA);
    TS_ASSERT_DELTA(obtainLineFromSvg().p1().y(), expected.p1().y(), DELTA);
    TS_ASSERT_DELTA(obtainLineFromSvg().p2().x(), expected.p2().x(), DELTA);
    TS_ASSERT_DELTA(obtainLineFromSvg().p2().y(), expected.p2().y(), DELTA);
  }

  void testBoundingRectWithoutParent() {
    QS_ASSERT_EQUALS(SAMPLE_LONE_PAIR.boundingRect(), QRectF());
  }

  void testBoundingRectWithParent() {
    LonePair *lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH, BoundingBoxLinker::atTopLeft());
    lp->setParentItem(atom);
    QS_ASSERT_EQUALS(lp->boundingRect().center(), atom->boundingRect().topLeft()); // TODO
  }

  void testXmlOutput() {
    QByteArray output;
    QXmlStreamWriter writer(&output);
    SAMPLE_LONE_PAIR.writeXml(writer);
    QS_ASSERT_EQUALS(output, LONE_PAIR_XML);
  }

  void testXmlInput() {
    QXmlStreamReader reader(LONE_PAIR_XML);
    LonePair lonePair(0, 0, 0);
    reader.readNextStartElement();
    lonePair.readXml(reader);
    QS_ASSERT_EQUALS(lonePair, SAMPLE_LONE_PAIR);
  }
};
