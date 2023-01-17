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

#include <alignment.h>
#include "xmlassertion.h"
#include "utilities.h"
#include <painting/textfield.h>

using namespace Molsketch;
using XmlAssert::assertThat;

const QFont ATOM_FONT("Times", 10);
const QFont SCRIPT_FONT("Times", 6);
const QPair<QFont, QFont> FONTS{qMakePair(ATOM_FONT, SCRIPT_FONT)};

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

  struct LabelValues {
    qreal x, y;
    int fontSize;
    QString text;
    bool lastChild;
    LabelValues(qreal x, qreal y, int fontSize, const QString &text, bool lastChild = true)
      : x(x), y(y), fontSize(fontSize), text(text), lastChild(lastChild) {}
  };

  void assertLabelAndBoundingBox(const QString &text, const QList<LabelValues> &labels,
                                 const QRectF &expectedBounds, const Alignment &alignment = Alignment::Right,
                                 int hAtomCount = 0, int charge = 0) {
    auto field = TextField::generateLabelForAtom(text, ATOM_FONT, alignment, hAtomCount, charge);

    auto svg = getLabelRenderedAsSvg(field);
    assertThat(svg)->hasNodes("svg/g/g/text")->times(labels.size());
    int parentIndex = 0;
    for (int i = 0 ; i < labels.size() ; ++i) {
      auto textNode = assertThat(svg)->hasNodes("svg/g/g/text")->index(i);
      textNode->haveTexts()->exactly({labels[i].text});
      textNode->haveAttribute("font-family")->exactly({"Times"});
      textNode->haveAttribute("font-size")->exactly({QString::number(labels[i].fontSize)});
      auto parentNode = assertThat(svg)->hasParentOf("svg/g/g/text")->index(parentIndex);
      parentNode->haveAttribute("transform")->exactly({QString("matrix(1,0,0,1,%1,%2)").arg(labels[i].x).arg(labels[i].y)});
      parentNode->haveAttribute("font-family")->exactly({"Times"});
      parentNode->haveAttribute("font-size")->exactly({QString::number(labels[i].fontSize)});
      parentIndex += (labels[i].lastChild ? 1 : 0);
    }
    // TODO get label into the attribute list
//    const QString LABEL_POS_AND_FONT_QUERY("//*:text/string-join((parent::*:g/@transform|@font-size|@font-family), ', '),//*:text/string(text())");

    auto actualBounds = field->boundingRect();
    delete field;
    QS_ASSERT_EQUALS(actualBounds, expectedBounds);
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
    assertLabelAndBoundingBox("test", {{-10, 5.5, 10, "test"}},
                                      {-TEST_LABEL_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testSimpleAtomLabelWithSubscript() {
    assertLabelAndBoundingBox("test3", {{-10, 5.5, 10,  "test"}, {10, 5.5, 6,  "3"}},
                                       {-TEST_LABEL_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testSimpleAtomLabelWithSubscriptMiddle() {
    assertLabelAndBoundingBox("te3st", {{-5.5, SVG_Y_ORIGIN, 10,  "te"}, {5.5, 5.5, 6,  "3"}, {9.5, SVG_Y_ORIGIN, 10,  "st"}},
                                       {-5.5, -ATOM_FONT_HEIGHT/2, 25, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testSimpleAtomLabelWithSubscriptBeginning() {
    assertLabelAndBoundingBox("3test", {{-10, 5.5, 10,  "test"}, {-14, 5.5, 6,  "3"}},
                                       {-14, -SVG_Y_SUBSCRIPT, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testAtomWithTrailingHLeft() {
    assertLabelAndBoundingBox("AB", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {-15.5, 5.5, 10,  "H"}},
                                     {-15.5, -9.5, 30, 19}, Alignment::Left, 1);
  }

  void testAtomWithTrailingHRight() {
    assertLabelAndBoundingBox("AB", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {14.5, 5.5, 10,  "H"}},
                                     {-5.5, -9.5, 30, 19}, Alignment::Right, 1);
  }

  void testAtomWithTrailingHDown() {
    assertLabelAndBoundingBox("AB", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {-5, 24.5, 10,  "H"}},
                                     {-5.5, -9.5, 20, 38}, Alignment::Down, 1);
  }

  void testAtomWithTrailingHUp() {
    assertLabelAndBoundingBox("AB", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {-5, -13.5, 10,  "H"}},
                                     {-5.5, -28.5, 20, 38}, Alignment::Up, 1);
  }

  void testWithCharge() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}},
                              {-5.5, -10.1, 16, 19.6}, Alignment::Right, 0, 1);
  }

  void testWithMultipleCharges() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "2+"}},
                              {-5.5, -10.1, 20, 19.6}, Alignment::Right, 0, 2);
  }

  void testWithNegativeCharge() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  QChar(0x2212)}},
                              {-5.5, -10.1, 16, 19.6}, Alignment::Right, 0, -1);
  }

  void testWithMultipleNegativeCharges() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  QString::number(2) + QChar(0x2212)}},
                              {-5.5, -10.1, 20, 19.6}, Alignment::Right, 0, -2);
  }

  void testWithMultipleElementsAndCharge() {
    assertLabelAndBoundingBox("AcBe", {{-8, 5.5, 10,  "Ac"}, {8, 5.5, 10,  "Be"}, {24, 5.5, 6,  "+"}},
                              {-8, -10.1, 37, 19.6}, Alignment::Right, 0, 1);
  }

  void testWithHatomRightAndCharge() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "H"}, {15.5, 5.5, 6,  "+"}},
                              {-5.5, -10.1, 26, 19.6}, Alignment::Right, 1, 1);
  }


  void testWithHatomsRightAndCharges() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "H"}, {15.5, 5.5, 6,  "2", false}, {15.5, 5.5, 6,  "2+"}},
                              {-5.5, -10.1, 30, 21.2}, Alignment::Right, 2, 2);
  }

  void testWithHatomLeftAndCharge() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}, {-15.5, 5.5, 10,  "H"}},
                              {-15.5, -10.1, 26, 19.6}, Alignment::Left, 1, 1);
  }

  void testWithHatomUpAndCharge() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}, {-5, -13.5, 10,  "H"}},
                              {-5.5, -29.1, 16, 38.6}, Alignment::Up, 1, 1);
  } //TODO: why is the left coordinate of H not -5.5? Align on bottom left/top left corner instead?

  void testWithHatomDownAndCharge() {
    assertLabelAndBoundingBox("A", {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}, {-5, 24.5, 10,  "H"}},
                              {-5.5, -10.1, 16, 38.6}, Alignment::Down, 1, 1);
  }
};
