/***************************************************************************
 *   Copyright (C) 2019 by Hendrik Vennekate                               *
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

#include <QBuffer>
#include <QPainter>
#include <QSvgGenerator>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlResultItems>

#include <atomlabelrenderer.h>
#include <alignment.h>
#include "xmlassertion.h"
#include "utilities.h"

using namespace Molsketch;
using XmlAssert::assertThat;

const QFont ATOM_FONT("Times", 10);
const QFont SCRIPT_FONT("Times", 7);
const QPair<QFont, QFont> FONTS{qMakePair(ATOM_FONT, SCRIPT_FONT)};
// TODO get label into the attribute list
const QString LABEL_POS_AND_FONT_QUERY("//*:text/string-join((@x|@y|@font-size|@font-family), ', '),//*:text/string(text())");

class AtomLabelRendererUnitTest : public CxxTest::TestSuite {
private:
  AtomLabelRenderer renderer;

  QString getLabelRenderedAsSvg(const QString &text, const Alignment &alignment) {
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QBuffer::WriteOnly);
    QSvgGenerator svgGenerator;
    svgGenerator.setOutputDevice(&buffer);
    QPainter painter;
    painter.begin(&svgGenerator);

    renderer.drawAtomLabel(&painter, text, FONTS, alignment);

    painter.end();
    buffer.close();
    return ba;
  }

  QString getLabelRenderedAsSvg(const QString &text) {
    return getLabelRenderedAsSvg(text, Alignment::Right);
  }

  void assertLabelAndBoundingBox(const QString &text, const QStringList &svgElements,
                                 const QRectF &expectedBounds, const Alignment &alignment = Alignment::Right) {
    auto svg = getLabelRenderedAsSvg(text, alignment);
    assertThat(svg)->contains(LABEL_POS_AND_FONT_QUERY)->exactly(svgElements);

    auto actualBounds = renderer.computeBoundingRect(text, FONTS, alignment);
    QS_ASSERT_EQUALS(actualBounds, expectedBounds);
  }
public:
  void testSimpleAtomLabel() {
    assertLabelAndBoundingBox("test", {"-2, 5.5, Times, 10", "test"}, {-2, -9.5, 19, 19});
  }

  void testSimpleAtomLabelWithSubscript() {
    assertLabelAndBoundingBox("test3", {"-2, 5.5, Times, 10", "17, 9.5, Times, 7", "test", "3"}, {-2, -9.5, 24, 19});
  }

  void testSimpleAtomLabelWithSubscriptMiddle() {
    assertLabelAndBoundingBox("te3st", {"-2, 5.5, Times, 10", "8, 9.5, Times, 7", "13, 5.5, Times, 10", "te", "3", "st"}, {-2, -9.5, 24, 19});
  }

  void testSimpleAtomLabelWithSubscriptBeginning() {
    assertLabelAndBoundingBox("3test", {"-3.5, 9.5, Times, 7", "1.5, 5.5, Times, 10", "3", "test"}, {-3.5, -9.5, 24, 19});
  }

  void testSimpleAtomLabelWithSubscriptMultiple() {
    assertLabelAndBoundingBox("te3st", {"-2, 5.5, Times, 10", "8, 9.5, Times, 7", "13, 5.5, Times, 10", "te", "3", "st"}, {-2, -9.5, 24, 19});
  }

  void testAtomWithTrailingHLeft() {
    assertLabelAndBoundingBox("ABH", {"-24, 5.5, Times, 10", "ABH"}, {-24, -9.5, 30, 19}, Alignment::Left);
  }

  void testAtomWithTrailingHRight() {
    assertLabelAndBoundingBox("ABH", {"-5, 5.5, Times, 10", "ABH"}, {-5, -9.5, 30, 19}, Alignment::Right);
  }

  void testAtomWithTrailingHDown() {
    assertLabelAndBoundingBox("ABH", {"-9.5, 5.5, Times, 10", "-9.5, 20.5, Times, 10", "AB", "H"}, {-9.5, -9.5, 19, 34}, Alignment::Down);
  }

  void testAtomWithTrailingHUp() {
    assertLabelAndBoundingBox("ABH", {"-9.5, 5.5, Times, 10", "-9.5, -9.5, Times, 10", "AB", "H"}, {-9.5, -24.5, 19, 34}, Alignment::Up);
  }

  void testAtomWithLeadingHLeft() {
    assertLabelAndBoundingBox("HAB", {"-24.5, 5.5, Times, 10", "HAB"}, {-24.5, -9.5, 30, 19}, Alignment::Left);
  }

  void testAtomWithLeadingHRight() {
    assertLabelAndBoundingBox("HAB", {"-5, 5.5, Times, 10", "HAB"}, {-5, -9.5, 30, 19}, Alignment::Right);
  }

  void testAtomWithLeadingHDown() {
    assertLabelAndBoundingBox("HAB", {"0, 5.5, Times, 10", "HAB"}, {0, -9.5, 30, 19}, Alignment::Down);
  }

  void testAtomWithLeadingHUp() {
    assertLabelAndBoundingBox("HAB", {"0, 5.5, Times, 10", "HAB"}, {0, -9.5, 30, 19}, Alignment::Up);
  }

  void testHAtomOnlyLeft() {
    assertLabelAndBoundingBox("H", {"-5, 5.5, Times, 10", "H"}, {-5, -9.5, 10, 19}, Alignment::Left);
  }

  void testHAtomOnlyRight() {
    assertLabelAndBoundingBox("H", {"-5, 5.5, Times, 10", "H"}, {-5, -9.5, 10, 19}, Alignment::Right);
  }

  void testHAtomOnlyUp() {
    assertLabelAndBoundingBox("H", {"-5, 5.5, Times, 10", "H"}, {-5, -9.5, 10, 19}, Alignment::Up);
  }

  void testHAtomOnlyDown() {
    assertLabelAndBoundingBox("H", {"-5, 5.5, Times, 10", "H"}, {-5, -9.5, 10, 19}, Alignment::Down);
  }

  void testH2OnlyLeft() {
    assertLabelAndBoundingBox("H2", {"-11.5, 5.5, Times, 10", "-1.5, 9.5, Times, 7", "H", "2"}, {-11.5, -9.5, 15, 19}, Alignment::Left);
  }

  void testH2OnlyRight() {
    assertLabelAndBoundingBox("H2", {"-5, 5.5, Times, 10", "5, 9.5, Times, 7", "H", "2"}, {-5, -9.5, 15, 19}, Alignment::Right);
  }

  void testH2OnlyUp() {
    assertLabelAndBoundingBox("H2", {"-7.5, 5.5, Times, 10", "2.5, 9.5, Times, 7", "H", "2"}, {-7.5, -9.5, 15, 19}, Alignment::Up);
  }

  void testH2OnlyDown() {
    assertLabelAndBoundingBox("H2", {"-7.5, 5.5, Times, 10", "2.5, 9.5, Times, 7", "H", "2"}, {-7.5, -9.5, 15, 19}, Alignment::Down);
  }

  void testStackedHHDown() { // This is actually buggy TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-4.5, 5.5, Times, 10", "-4.5, 20.5, Times, 10", "-4.5, 35.5, Times, 10", "C", "H", "H"}, {-4.5, -9.5, 10, 49}, Alignment::Down);
  }

  void testStackedHHUp() { // This is actually buggy TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-4.5, 5.5, Times, 10", "-4.5, -9.5, Times, 10", "-4.5, -24.5, Times, 10", "C", "H", "H"}, {-4.5, -39.5, 10, 49}, Alignment::Up);
  }

  void testStackedHHLeft() { // This is actually buggy (bounding rect) TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-24, 5.5, Times, 10", "CHH"}, {-24, -9.5, 30, 19}, Alignment::Left);
  }

  void testStackedHHRight() {  // This is actually buggy (bounding rect) TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-4.5, 5.5, Times, 10", "CHH"}, {-4.5, -9.5, 30, 19}, Alignment::Right);
  }
};
