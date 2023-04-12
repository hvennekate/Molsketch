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
#include "xmlassertion.h"

using namespace Molsketch;

const qreal LENGTH = 10;
const qreal ANGLE = 45;
const qreal LINE_WIDTH = 1.5;
const BoundingBoxLinker ANCHOR = BoundingBoxLinker::atTopLeft();
const QString &LINE_QUERY("svg/g/g/polyline");
const LonePair SAMPLE_LONE_PAIR(23.5, 1.5, 5.5, BoundingBoxLinker::atBottomLeft(), Qt::blue);
const QString LONE_PAIR_XML("<lonePair angle=\"23.5\" length=\"5.5\" lineWidth=\"1.5\" colorR=\"0\" colorG=\"0\" colorB=\"255\">"
                            "<bbLinker originAnchor=\"BottomLeft\" targetAnchor=\"Center\" xOffset=\"0\" yOffset=\"0\"/>"
                            "</lonePair>");

using XmlAssert::assertThat;

class LonePairUnitTest : public CxxTest::TestSuite {
  MolScene *scene;
  Atom *atom;

  QByteArray svgOfLine() {
    auto lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH, ANCHOR);
    lp->setParentItem(atom);
    return scene->toSvg();
  }

  QByteArray svgOfRedLine() {
    auto lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH, BoundingBoxLinker::atTop(), Qt::red);
    lp->setParentItem(atom);
    return scene->toSvg();
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
    assertThat(scene->toSvg())->hasNodes(LINE_QUERY)->none();;
  }

  void testLonePairDrawnIfParentIsAtom() {
    LonePair *lp = new LonePair(ANGLE, LINE_WIDTH, LENGTH);
    lp->setParentItem(atom);
    assertThat(scene->toSvg())->hasNodes(LINE_QUERY)->exactlyOne();
  }

  void testLinePointCoordinates() {
    assertThat(svgOfLine())->hasNodes(LINE_QUERY)
        ->exactlyOne()->haveAttribute("points")->exactly(
      #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
          {"-6.53553,-2.96447 0.535534,-10.0355 "}
      #else
          {"-6.53553,-2.59728 0.535534,-9.66835 "}
      #endif
          );
  }

  void testLinewidthAttribute() {
    assertThat(svgOfLine())->hasParentOf(LINE_QUERY)
        ->exactlyOne()->haveAttribute("stroke-width")->exactly({QString::number(LINE_WIDTH)});
  }

  void testColorAttribute() {
    assertThat(svgOfRedLine())->hasParentOf(LINE_QUERY)
        ->exactlyOne()->haveAttribute("stroke")->exactly({"#ff0000"});
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
