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

#include <alignment.h>
#include "xmlassertion.h"
#include "utilities.h"
#include <painting/textfield.h>

using namespace Molsketch;
using XmlAssert::assertThat;

const QFont ATOM_FONT("Times", 10);
const QFont SCRIPT_FONT("Times", 6);
const QPair<QFont, QFont> FONTS{qMakePair(ATOM_FONT, SCRIPT_FONT)};
// TODO get label into the attribute list
const QString LABEL_POS_AND_FONT_QUERY("//*:text/string-join((parent::*:g/@transform|@font-size|@font-family), ', '),//*:text/string(text())");

class AtomLabelRendererUnitTest : public CxxTest::TestSuite {
private:

  QFontMetricsF ATOM_FONT_METRICS, SCRIPT_FONT_METRICS;
  qreal TEST_LABEL_WIDTH, TEST3_LABEL_WIDTH, TE_LABEL_WIDTH, ATOM_FONT_HEIGHT, ATOM_FONT_HEIGHT_WITH_SUB, LETTER_T_WIDTH, LETTER_T_WITH_3_WIDTH, AB_WIDTH, H_WIDTH, H2_WIDTH, TWO_LINE_HEIGHT, THREE_LINE_HEIGHT, SVG_Y_ORIGIN, SVG_Y_SUBSCRIPT, SVG_LETTER_T_WIDTH, SVG_TEST_WIDTH, SVG_TE_WIDTH, SVG_TE3_WIDTH, SVG_X_LEADING_SUB, SVG_X_AFTER_SUB, SVG_AB_WIDTH, SVG_Y_SECOND_LINE, SVG_Y_THIRD_LINE;


  QString getLabelRenderedAsSvg(const Paintable *paintable) {
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QBuffer::WriteOnly);
    QSvgGenerator svgGenerator;
    svgGenerator.setOutputDevice(&buffer);
    QPainter painter;
    painter.begin(&svgGenerator);

    paintable->paint(&painter);

    painter.end();
    buffer.close();
    return ba;
  }

  void assertLabelAndBoundingBox(const QString &text, const QStringList &svgElements,
                                 const QRectF &expectedBounds, const Alignment &alignment = Alignment::Right, int hAtomCount = 0) {
    auto field = TextField::generateLabelForAtom(text, ATOM_FONT, alignment, hAtomCount);

    auto svg = getLabelRenderedAsSvg(field);
    assertThat(svg)->contains(LABEL_POS_AND_FONT_QUERY)->exactly(svgElements);

    auto actualBounds = field->boundingRect();
    delete field;
    QS_ASSERT_EQUALS(actualBounds, expectedBounds);
  }

  QString svgDimensions(qreal x, qreal y, int fontSize) {
    return QString("matrix(1,0,0,1,%1,%2), Times, %3").arg(x).arg(y).arg(fontSize);
  }
public:
  AtomLabelRendererUnitTest()
    : ATOM_FONT_METRICS(ATOM_FONT),
      SCRIPT_FONT_METRICS(SCRIPT_FONT)
  {}

  void setUp() override {
    TEST_LABEL_WIDTH = ATOM_FONT_METRICS.boundingRect("test").width();
    TEST3_LABEL_WIDTH = TEST_LABEL_WIDTH + SCRIPT_FONT_METRICS.boundingRect("3").width();
    TE_LABEL_WIDTH = ATOM_FONT_METRICS.boundingRect("te").width();
    ATOM_FONT_HEIGHT = ATOM_FONT_METRICS.height();
    ATOM_FONT_HEIGHT_WITH_SUB = ATOM_FONT_HEIGHT + 1.6;
    LETTER_T_WIDTH = ATOM_FONT_METRICS.width("t");
    LETTER_T_WITH_3_WIDTH = -3.4921875;
    AB_WIDTH = ATOM_FONT_METRICS.width("AB");
    H_WIDTH = ATOM_FONT_METRICS.width("H");
    H2_WIDTH = H_WIDTH + SCRIPT_FONT_METRICS.width("2");
    TWO_LINE_HEIGHT = ATOM_FONT_HEIGHT + ATOM_FONT_METRICS.ascent();
    THREE_LINE_HEIGHT = TWO_LINE_HEIGHT + ATOM_FONT_METRICS.ascent();
    SVG_Y_ORIGIN = ATOM_FONT_HEIGHT/2 - ATOM_FONT_METRICS.descent();
    SVG_Y_SUBSCRIPT = 9.5;
    SVG_TE_WIDTH = 11;
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
    assertLabelAndBoundingBox("test", {svgDimensions(-10, 5.5, 10), "test"},
                                      {-TEST_LABEL_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testSimpleAtomLabelWithSubscript() {
    assertLabelAndBoundingBox("test3", {svgDimensions(-10, 5.5, 10), svgDimensions(10, 5.5, 6), "test", "3"},
                                       {-TEST_LABEL_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testSimpleAtomLabelWithSubscriptMiddle() {
    assertLabelAndBoundingBox("te3st", {svgDimensions(-5.5, SVG_Y_ORIGIN, 10),
                                        svgDimensions(5.5, 5.5, 6),
                                        svgDimensions(9.5, SVG_Y_ORIGIN, 10), "te", "3", "st"},
                                       {-5.5, -ATOM_FONT_HEIGHT/2, 25, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testSimpleAtomLabelWithSubscriptBeginning() {
    assertLabelAndBoundingBox("3test", {svgDimensions(-10, 5.5, 10),
                                        svgDimensions(-14, 5.5, 6), "test", "3"},
                                       {-14, -SVG_Y_SUBSCRIPT, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testAtomWithTrailingHLeft() {
    assertLabelAndBoundingBox("AB", {svgDimensions(-5.5, 5.5, 10), svgDimensions(5.5, 5.5, 10), svgDimensions(-15.5, 5.5, 10), "A", "B", "H"},
                                     {-15.5, -9.5, 30, 19}, Alignment::Left, 1);
  }

  void testAtomWithTrailingHRight() {
    assertLabelAndBoundingBox("AB", {svgDimensions(-5.5, 5.5, 10), svgDimensions(5.5, 5.5, 10), svgDimensions(14.5, 5.5, 10), "A", "B", "H"},
                                     {-5.5, -9.5, 30, 19}, Alignment::Right, 1);
  }

  void testAtomWithTrailingHDown() {
    assertLabelAndBoundingBox("AB", {svgDimensions(-5.5, 5.5, 10), svgDimensions(5.5, 5.5, 10), svgDimensions(-5, 24.5, 10), "A", "B", "H"},
                                     {-5.5, -9.5, 20, 38}, Alignment::Down, 1);
  }

  void testAtomWithTrailingHUp() {
    assertLabelAndBoundingBox("AB", {svgDimensions(-5.5, 5.5, 10), svgDimensions(5.5, 5.5, 10), svgDimensions(-5, -13.5, 10), "A", "B", "H"},
                                     {-5.5, -28.5, 20, 38}, Alignment::Up, 1);
  }

  // TODO charges
};
