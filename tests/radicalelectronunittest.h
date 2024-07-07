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
#include <radicalelectron.h>
#include "utilities.h"
#include <scenesettings.h>
#include <settingsitem.h>
#include "xmlassertion.h"

using namespace Molsketch;
using XmlAssert::assertThat;

CLASS_FOR_TESTING_WITH_FUNCTIONS(RadicalElectron, \
                                 public: \
                                 explicit RadicalElectronForTesting(qreal diameter, BoundingBoxLinker linker = BoundingBoxLinker(Anchor::Top, Anchor::Bottom), const QColor& color = QColor()) \
                                 : RadicalElectron(diameter, linker, color){})
const int DIAMETER = 2;
const QString RADICAL_ELECTRON_XML("<radicalElectron"
                                   " diameter=\"5\""
                                   " colorR=\"0\"" // TODO set back to 255 once color is serialized properly in XML
                                   " colorG=\"0\""
                                   " colorB=\"0\">"
                                   "<bbLinker"
                                   " originAnchor=\"TopLeft\""
                                   " targetAnchor=\"BottomRight\""
                                   " xOffset=\"0\""
                                   " yOffset=\"0\"/>"
                                   "</radicalElectron>");
const RadicalElectron SAMPLE_RADICAL_ELECTRON(5, BoundingBoxLinker::upperLeft(), Qt::black); // TODO restore if color is properly serialized , Qt::red);
const QString &CIRCLE_QUERY("svg/g/g/circle");

class RadicalElectronUnitTest : public CxxTest::TestSuite {
  MolScene *scene;
  Atom *atom;

  QString svgWithAtomAndRadicalElectron() {
    RadicalElectron *radical = new RadicalElectron(DIAMETER, BoundingBoxLinker::above(), Qt::blue);
    radical->setParentItem(atom);
    return scene->toSvg();
  }

public:
  void setUp() {
    scene = new MolScene;
    scene->settings()->atomFont()->set(QFont("Noto Sans", 8));
    atom = new Atom(QPointF(0,0), "C", false);
    atom->setCharge(0);
    scene->addItem(atom);
  }

  void tearDown() {
    delete scene;
    TSM_ASSERT_EQUALS("Radical electrons should be deleted properly!", RadicalElectronForTesting::instanceCounter, 0);
  }

  void testNothingDrawnIfNoParentWasSet() {
    scene->addItem(new RadicalElectron(DIAMETER));
    assertThat(scene->toSvg())->hasNodes(CIRCLE_QUERY)->none();
  }

  void testCircleIsDrawnIfParentIsAtom() {
    assertThat(svgWithAtomAndRadicalElectron())->hasNodes(CIRCLE_QUERY)->exactlyOne();
  }

  void testCircleSize() {
    assertThat(svgWithAtomAndRadicalElectron())->hasNodes(CIRCLE_QUERY)
        ->haveAttribute("r")->exactly({QString::number(DIAMETER/2.)});
  }

  void testPosition() {
    auto circle = assertThat(svgWithAtomAndRadicalElectron())->hasNodes(CIRCLE_QUERY);
    circle->haveAttribute("cx")->exactly({"0"});
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    circle->haveAttribute("cy")->exactly({"-9"});
#else
    circle->haveAttribute("cy")->exactly({"-8.49219"});
#endif
  }

  void assertSvgColor(const XmlNodesAssertion *parentAssertion, const QString &color) {
    parentAssertion->haveAttribute("fill")->exactly({color});
    parentAssertion->haveAttribute("stroke")->exactly({color});
  }

  void testColorWithExplicitColor_isExcplicitColor() {
    TS_SKIP("Need to first serialize validity of color to XML");
    auto parent = assertThat(svgWithAtomAndRadicalElectron())->hasParentOf(CIRCLE_QUERY);
    assertSvgColor(parent, "#0000ff");
  }

  void testColorWithoutParent_isNotDrawn() {
    auto radical = new RadicalElectron(DIAMETER);
    scene->addItem(radical);
    assertThat(scene->toSvg())->hasNodes(CIRCLE_QUERY)->none();
  }

  void testColorWithParentNoExplicitColor_isParentsColor() {
    RadicalElectron *radical = new RadicalElectron(DIAMETER, BoundingBoxLinker::above());
    radical->setParentItem(atom);
    atom->setColor(Qt::red);
    auto parent = assertThat(scene->toSvg())->hasParentOf(CIRCLE_QUERY);
    assertSvgColor(parent, "#ff0000");
  }

  void testColorWithParentAndExplicitColor_isExplicitColor() {
    TS_SKIP("Need to first serialize validity of color to XML");
    atom->setColor(Qt::red);
    auto parent = assertThat(svgWithAtomAndRadicalElectron())->hasParentOf(CIRCLE_QUERY);
    assertSvgColor(parent, "#0000ff");
  }

  void testBoundingRectWithoutParent() {
    QS_ASSERT_EQUALS(RadicalElectron(DIAMETER).boundingRect(), QRectF());
  }

  void testAdaptationOfBoundingRectToParentAndDeletionWithParent() {
    QPointF offset(2,3);
    RadicalElectron *radical = new RadicalElectronForTesting(DIAMETER, BoundingBoxLinker(Anchor::TopRight, Anchor::BottomLeft, offset));
    radical->setParentItem(atom);
    QRectF boundingRect = radical->boundingRect();
    QS_ASSERT_EQUALS(boundingRect.bottomLeft(), atom->boundingRect().topRight() + offset);
    QS_ASSERT_EQUALS(boundingRect.width(), DIAMETER);
    QS_ASSERT_EQUALS(boundingRect.height(), DIAMETER);
  }

  void testXmlOutput() {
    QByteArray output; // TODO write standardized output test routine
    QXmlStreamWriter writer(&output);
    SAMPLE_RADICAL_ELECTRON.writeXml(writer);
    QS_ASSERT_EQUALS(output, RADICAL_ELECTRON_XML);
  }

  void testXmlInput() {
    QXmlStreamReader reader(RADICAL_ELECTRON_XML);
    RadicalElectron radicalElectron(DIAMETER);
    reader.readNextStartElement();
    radicalElectron.readXml(reader);
    QS_ASSERT_EQUALS(radicalElectron, SAMPLE_RADICAL_ELECTRON);
  }
};
