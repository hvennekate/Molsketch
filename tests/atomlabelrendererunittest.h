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
    assertLabelAndBoundingBox("test", {"-1.9375, 4.5, Times, 10", "test"}, {-1.9375, -8.5, 19.390625, 17});
  }

  void testSimpleAtomLabelWithSubscript() {
    assertLabelAndBoundingBox("test3", {"-1.9375, 4.5, Times, 10", "17.4531, 8.5, Times, 7", "test", "3"}, {-1.9375, -8.5, 24.390625, 17});
  }

  void testSimpleAtomLabelWithSubscriptMiddle() {
    assertLabelAndBoundingBox("te3st", {"-1.9375, 4.5, Times, 10", "8.14063, 8.5, Times, 7", "13.1406, 4.5, Times, 10", "te", "3", "st"}, {-1.9375, -8.5, 24.390625, 17});
  }

  void testSimpleAtomLabelWithSubscriptBeginning() {
    assertLabelAndBoundingBox("3test", {"-3.5, 8.5, Times, 7", "1.5, 4.5, Times, 10", "3", "test"}, {-3.5, -8.5, 24.390625, 17});
  }

  void testSimpleAtomLabelWithSubscriptMultiple() {
    assertLabelAndBoundingBox("te3st", {"-1.9375, 4.5, Times, 10", "8.14063, 8.5, Times, 7", "13.1406, 4.5, Times, 10", "te", "3", "st"}, {-1.9375, -8.5, 24.390625, 17});
  }

  void testAtomWithTrailingHLeft() {
    assertLabelAndBoundingBox("ABH", {"-24.4844, 4.5, Times, 10", "ABH"}, {-24.4921875, -8.5, 29.546875, 17}, Alignment::Left);
  }

  void testAtomWithTrailingHRight() {
    assertLabelAndBoundingBox("ABH", {"-5.04688, 4.5, Times, 10", "ABH"}, {-5.0546875, -8.5, 29.546875, 17}, Alignment::Right);
  }

  void testAtomWithTrailingHDown() {
    assertLabelAndBoundingBox("ABH", {"-9.71875, 4.5, Times, 10", "-9.71875, 17.5, Times, 10", "AB", "H"}, {-9.71875, -8.5, 19.4375, 30}, Alignment::Down);
  }

  void testAtomWithTrailingHUp() {
    assertLabelAndBoundingBox("ABH", {"-9.71875, 4.5, Times, 10", "-9.71875, -8.5, Times, 10", "AB", "H"}, {-9.71875, -21.5, 19.4375, 30}, Alignment::Up);
  }

  void testAtomWithLeadingHLeft() {
    assertLabelAndBoundingBox("HAB", {"-24.875, 4.5, Times, 10", "HAB"}, {-24.8828125, -8.5, 29.546875, 17}, Alignment::Left);
  }

  void testAtomWithLeadingHRight() {
    assertLabelAndBoundingBox("HAB", {"-5.04688, 4.5, Times, 10", "HAB"}, {-5.0546875, -8.5, 29.546875, 17}, Alignment::Right);
  }

  void testAtomWithLeadingHDown() {
    assertLabelAndBoundingBox("HAB", {"0, 4.5, Times, 10", "HAB"}, {0, -8.5, 29.546875, 17}, Alignment::Down);
  }

  void testAtomWithLeadingHUp() {
    assertLabelAndBoundingBox("HAB", {"0, 4.5, Times, 10", "HAB"}, {0, -8.5, 29.546875, 17}, Alignment::Up);
  }

  void testHAtomOnlyLeft() {
    assertLabelAndBoundingBox("H", {"-5.04688, 4.5, Times, 10", "H"}, {-5.0546875, -8.5, 10.109375, 17}, Alignment::Left);
  }

  void testHAtomOnlyRight() {
    assertLabelAndBoundingBox("H", {"-5.04688, 4.5, Times, 10", "H"}, {-5.0546875, -8.5, 10.109375, 17}, Alignment::Right);
  }

  void testHAtomOnlyUp() {
    assertLabelAndBoundingBox("H", {"-5.04688, 4.5, Times, 10", "H"}, {-5.0546875, -8.5, 10.109375, 17}, Alignment::Up);
  }

  void testHAtomOnlyDown() {
    assertLabelAndBoundingBox("H", {"-5.04688, 4.5, Times, 10", "H"}, {-5.0546875, -8.5, 10.109375, 17}, Alignment::Down);
  }

  void testH2OnlyLeft() {
    assertLabelAndBoundingBox("H2", {"-11.6094, 4.5, Times, 10", "-1.5, 8.5, Times, 7", "H", "2"}, {-11.609375, -8.5, 15.109375, 17}, Alignment::Left);
  }

  void testH2OnlyRight() {
    assertLabelAndBoundingBox("H2", {"-5.04688, 4.5, Times, 10", "5.04688, 8.5, Times, 7", "H", "2"}, {-5.0546875, -8.5, 15.109375, 17}, Alignment::Right);
  }

  void testH2OnlyUp() {
    assertLabelAndBoundingBox("H2", {"-7.54688, 4.5, Times, 10", "2.54688, 8.5, Times, 7", "H", "2"}, {-7.5546875, -8.5, 15.109375, 17}, Alignment::Up);
  }

  void testH2OnlyDown() {
    assertLabelAndBoundingBox("H2", {"-7.54688, 4.5, Times, 10", "2.54688, 8.5, Times, 7", "H", "2"}, {-7.5546875, -8.5, 15.109375, 17}, Alignment::Down);
  }

  void testStackedHHDown() { // This is actually buggy TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-4.65625, 4.5, Times, 10", "-4.65625, 17.5, Times, 10", "-4.65625, 30.5, Times, 10", "C", "H", "H"}, {-4.6640625, -8.5, 10.109375, 43}, Alignment::Down);
  }

  void testStackedHHUp() { // This is actually buggy TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-4.65625, 4.5, Times, 10", "-4.65625, -8.5, Times, 10", "-4.65625, -21.5, Times, 10", "C", "H", "H"}, {-4.6640625, -34.5, 10.109375, 43}, Alignment::Up);
  }

  void testStackedHHLeft() { // This is actually buggy (bounding rect) TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-24.4844, 4.5, Times, 10", "CHH"}, {-24.4921875, -8.5, 29.546875, 17}, Alignment::Left);
  }

  void testStackedHHRight() {  // This is actually buggy (bounding rect) TOOD test visually!
    assertLabelAndBoundingBox("CHH", {"-4.65625, 4.5, Times, 10", "CHH"}, {-4.6640625, -8.5, 29.546875, 17}, Alignment::Right);
  }
};
