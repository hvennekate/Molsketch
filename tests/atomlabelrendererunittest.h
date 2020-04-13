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

  QFontMetricsF ATOM_FONT_METRICS, SCRIPT_FONT_METRICS;
  qreal TEST_LABEL_WIDTH, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT, LETTER_T_WIDTH, LETTER_T_WITH_3_WIDTH, AB_WIDTH, H_WIDTH, H2_WIDTH, TWO_LINE_HEIGHT, THREE_LINE_HEIGHT, SVG_Y_ORIGIN, SVG_Y_SUBSCRIPT, SVG_LETTER_T_WIDTH, SVG_TEST_WIDTH, SVG_TE_WIDTH, SVG_TE3_WIDTH, SVG_X_LEADING_SUB, SVG_X_AFTER_SUB, SVG_AB_WIDTH, SVG_Y_SECOND_LINE, SVG_Y_THIRD_LINE;


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

  QString svgDimensions(qreal x, qreal y, int fontSize) {
    return QString("%1, %2, Times, %3").arg(x).arg(y).arg(fontSize);
  }
public:
  AtomLabelRendererUnitTest()
    : ATOM_FONT_METRICS(ATOM_FONT),
      SCRIPT_FONT_METRICS(SCRIPT_FONT)
  {}

  void setUp() override {
    TEST_LABEL_WIDTH = ATOM_FONT_METRICS.width("test");
    TEST3_LABEL_WIDTH = TEST_LABEL_WIDTH + SCRIPT_FONT_METRICS.width("3");
    ATOM_FONT_HEIGHT = ATOM_FONT_METRICS.height();
    LETTER_T_WIDTH = ATOM_FONT_METRICS.width("t");
    LETTER_T_WITH_3_WIDTH = -3.4921875;
    AB_WIDTH = ATOM_FONT_METRICS.width("AB");
    H_WIDTH = ATOM_FONT_METRICS.width("H");
    H2_WIDTH = H_WIDTH + SCRIPT_FONT_METRICS.width("2");
    TWO_LINE_HEIGHT = ATOM_FONT_HEIGHT + ATOM_FONT_METRICS.ascent();
    THREE_LINE_HEIGHT = TWO_LINE_HEIGHT + ATOM_FONT_METRICS.ascent();
    SVG_Y_ORIGIN = ATOM_FONT_HEIGHT/2 - ATOM_FONT_METRICS.descent();
    SVG_Y_SUBSCRIPT = SVG_Y_ORIGIN + ATOM_FONT_METRICS.descent();
    SVG_LETTER_T_WIDTH = 1.9375*2;
    SVG_TEST_WIDTH = 17.25;
    SVG_TE_WIDTH = 8.14063;
    SVG_TE3_WIDTH = 13.1406;
    SVG_X_LEADING_SUB = -3.48438;
    SVG_X_AFTER_SUB = 1.5;
    SVG_AB_WIDTH = 9.703125*2;
    SVG_Y_SECOND_LINE = SVG_Y_ORIGIN + ATOM_FONT_METRICS.ascent();
    SVG_Y_THIRD_LINE = SVG_Y_SECOND_LINE + ATOM_FONT_METRICS.ascent();
  }

  void testSimpleAtomLabel() {
    assertLabelAndBoundingBox("test", {svgDimensions(-SVG_LETTER_T_WIDTH/2, SVG_Y_ORIGIN, 10), "test"},
                                      {-LETTER_T_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testSimpleAtomLabelWithSubscript() {
    assertLabelAndBoundingBox("test3", {svgDimensions(-SVG_LETTER_T_WIDTH/2, SVG_Y_ORIGIN, 10),
                                        svgDimensions(SVG_TEST_WIDTH, SVG_Y_SUBSCRIPT, 7), "test", "3"},
                                       {-LETTER_T_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testSimpleAtomLabelWithSubscriptMiddle() {
    assertLabelAndBoundingBox("te3st", {svgDimensions(-SVG_LETTER_T_WIDTH/2, SVG_Y_ORIGIN, 10),
                                        svgDimensions(SVG_TE_WIDTH, SVG_Y_SUBSCRIPT, 7),
                                        svgDimensions(SVG_TE3_WIDTH, SVG_Y_ORIGIN, 10), "te", "3", "st"},
                                       {-LETTER_T_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testSimpleAtomLabelWithSubscriptBeginning() {
    assertLabelAndBoundingBox("3test", {svgDimensions(SVG_X_LEADING_SUB, SVG_Y_SUBSCRIPT, 7),
                                        svgDimensions(SVG_X_AFTER_SUB, SVG_Y_ORIGIN, 10), "3", "test"},
                                       {LETTER_T_WITH_3_WIDTH, -SVG_Y_SUBSCRIPT, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testAtomWithTrailingHLeft() {
    assertLabelAndBoundingBox("ABH", {svgDimensions(-AB_WIDTH - H_WIDTH/2, SVG_Y_ORIGIN, 10), "ABH"},
                                     {-AB_WIDTH - H_WIDTH/2, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Left);
  }

  void testAtomWithTrailingHRight() {
    assertLabelAndBoundingBox("ABH", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10), "ABH"},
                                     {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Right);
  }

  void testAtomWithTrailingHDown() {
    assertLabelAndBoundingBox("ABH", {svgDimensions(-SVG_AB_WIDTH/2, SVG_Y_ORIGIN, 10),
                                      svgDimensions(-SVG_AB_WIDTH/2, SVG_Y_SECOND_LINE, 10), "AB", "H"},
                                     {-AB_WIDTH/2, -SVG_Y_SUBSCRIPT, AB_WIDTH, TWO_LINE_HEIGHT}, Alignment::Down);
  }

  void testAtomWithTrailingHUp() {
    assertLabelAndBoundingBox("ABH", {svgDimensions(-SVG_AB_WIDTH/2, SVG_Y_ORIGIN, 10),
                                      svgDimensions(-SVG_AB_WIDTH/2, -SVG_Y_SUBSCRIPT, 10), "AB", "H"},
                                     {-AB_WIDTH/2, -ATOM_FONT_METRICS.height()/2 - ATOM_FONT_METRICS.ascent(), AB_WIDTH, TWO_LINE_HEIGHT}, Alignment::Up);
  }

  void testAtomWithLeadingHLeft() {
    assertLabelAndBoundingBox("HAB", {svgDimensions(-24.8438, SVG_Y_ORIGIN, 10), "HAB"},
                                     {-24.8515625, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Left);
  }

  void testAtomWithLeadingHRight() {
    assertLabelAndBoundingBox("HAB", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10), "HAB"},
                                     {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Right);
  }

  void testAtomWithLeadingHDown() {
    assertLabelAndBoundingBox("HAB", {svgDimensions(0, SVG_Y_ORIGIN, 10), "HAB"},
                                     {0, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Down);
  }

  void testAtomWithLeadingHUp() {
    assertLabelAndBoundingBox("HAB", {svgDimensions(0, SVG_Y_ORIGIN, 10), "HAB"},
                                     {0, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Up);
  }

  void testHAtomOnlyLeft() {
    assertLabelAndBoundingBox("H", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10), "H"},
                                   {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Left);
  }

  void testHAtomOnlyRight() {
    assertLabelAndBoundingBox("H", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10), "H"},
                                   {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Right);
  }

  void testHAtomOnlyUp() {
    assertLabelAndBoundingBox("H", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10), "H"},
                                   {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Up);
  }

  void testHAtomOnlyDown() {
    assertLabelAndBoundingBox("H", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10), "H"},
                                   {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Down);
  }

  void testH2OnlyLeft() {
    assertLabelAndBoundingBox("H2", {svgDimensions(-11.5938, SVG_Y_ORIGIN, 10),
                                     svgDimensions(-1.5, SVG_Y_SUBSCRIPT, 7), "H", "2"},
                                    {-11.6015625, -SVG_Y_SUBSCRIPT, H2_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Left);
  }

  void testH2OnlyRight() {
    assertLabelAndBoundingBox("H2", {svgDimensions(-H_WIDTH/2, SVG_Y_ORIGIN, 10),
                                     svgDimensions(5.04688, SVG_Y_SUBSCRIPT, 7), "H", "2"},
                                    {-H_WIDTH/2, -SVG_Y_SUBSCRIPT, H2_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Right);
  }

  void testH2OnlyUp() {
    assertLabelAndBoundingBox("H2", {svgDimensions(-7.54688, SVG_Y_ORIGIN, 10),
                                     svgDimensions(2.54688, SVG_Y_SUBSCRIPT, 7), "H", "2"},
                                    {-H2_WIDTH/2, -SVG_Y_SUBSCRIPT, H2_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Up);
  }

  void testH2OnlyDown() {
    assertLabelAndBoundingBox("H2", {svgDimensions(-7.54688, SVG_Y_ORIGIN, 10),
                                     svgDimensions(2.54688, SVG_Y_SUBSCRIPT, 7), "H", "2"},
                                    {-H2_WIDTH/2, -SVG_Y_SUBSCRIPT, H2_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Down);
  }

  void testStackedHHDown() { // This is actually buggy TOOD test visually!
    assertLabelAndBoundingBox("CHH", {svgDimensions(-4.65625, SVG_Y_ORIGIN, 10),
                                      svgDimensions(-4.65625, SVG_Y_SECOND_LINE, 10),
                                      svgDimensions(-4.65625, SVG_Y_THIRD_LINE, 10), "C", "H", "H"},
                                     {-4.6640625, -SVG_Y_SUBSCRIPT, H_WIDTH, THREE_LINE_HEIGHT}, Alignment::Down);
  }

  void testStackedHHUp() { // This is actually buggy TOOD test visually!
    assertLabelAndBoundingBox("CHH", {svgDimensions(-4.65625, SVG_Y_ORIGIN, 10),
                                      svgDimensions(-4.65625, -SVG_Y_SUBSCRIPT, 10),
                                      svgDimensions(-4.65625, -SVG_Y_SUBSCRIPT - ATOM_FONT_METRICS.ascent(), 10), "C", "H", "H"},
                                     {-4.6640625, SVG_Y_ORIGIN - 3 * ATOM_FONT_METRICS.ascent(), H_WIDTH, THREE_LINE_HEIGHT}, Alignment::Up);
  }

  void testStackedHHLeft() { // This is actually buggy (bounding rect) TOOD test visually!
    assertLabelAndBoundingBox("CHH", {svgDimensions(-AB_WIDTH - H_WIDTH/2, SVG_Y_ORIGIN, 10), "CHH"},
                                     {-AB_WIDTH - H_WIDTH/2, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Left);
  }

  void testStackedHHRight() {  // This is actually buggy (bounding rect) TOOD test visually!
    assertLabelAndBoundingBox("CHH", {svgDimensions(-4.65625, SVG_Y_ORIGIN, 10), "CHH"},
                                     {-4.6640625, -SVG_Y_SUBSCRIPT, AB_WIDTH + H_WIDTH, ATOM_FONT_HEIGHT}, Alignment::Right);
  }
};
