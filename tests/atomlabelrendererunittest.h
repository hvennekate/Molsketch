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

const QFont ATOM_FONT("Times", 10, 50);
const QFont SCRIPT_FONT("Times", 6, 50);
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


  // TODO why do values differ in Qt6?
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#define MSK_QT5
#endif
  void setUp() override {
    TEST_LABEL_WIDTH = ATOM_FONT_METRICS.boundingRect("test").width();
    TEST3_LABEL_WIDTH = TEST_LABEL_WIDTH + SCRIPT_FONT_METRICS.boundingRect("3").width();
    TE_LABEL_WIDTH = ATOM_FONT_METRICS.boundingRect("te").width();
    ATOM_FONT_HEIGHT = ATOM_FONT_METRICS.height();
#ifdef MSK_QT5
    ATOM_FONT_HEIGHT_WITH_SUB = ATOM_FONT_HEIGHT + 1.6;
#else
    ATOM_FONT_HEIGHT_WITH_SUB = ATOM_FONT_HEIGHT + 0.871875;
#endif
    LETTER_T_WIDTH = ATOM_FONT_METRICS.boundingRect("t").width();
    LETTER_T_WITH_3_WIDTH = -3.4921875;
    AB_WIDTH = ATOM_FONT_METRICS.boundingRect("AB").width();
    H_WIDTH = ATOM_FONT_METRICS.boundingRect("H").width();
    H2_WIDTH = H_WIDTH + SCRIPT_FONT_METRICS.boundingRect("2").width();
    TWO_LINE_HEIGHT = ATOM_FONT_HEIGHT + ATOM_FONT_METRICS.ascent();
    THREE_LINE_HEIGHT = TWO_LINE_HEIGHT + ATOM_FONT_METRICS.ascent();
    SVG_Y_ORIGIN = ATOM_FONT_HEIGHT/2 - ATOM_FONT_METRICS.descent();
#ifdef MSK_QT5
    SVG_Y_SUBSCRIPT = 9.5;
#else
    SVG_Y_SUBSCRIPT = 9.3515625;
#endif
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

  void testSimpleAtomLabel() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("test",
                          #ifdef MSK_QT5
                              {{-10, 5.5, 10, "test"}},
                          #else
                              {{-10.5, 5.41406, 10, "test"}},
                          #endif
                                      {-TEST_LABEL_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST_LABEL_WIDTH, ATOM_FONT_HEIGHT});
  }

  void testSimpleAtomLabelWithSubscript() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("test3",
                          #ifdef MSK_QT5
                              {{-10, 5.5, 10,  "test"}, {10, 5.5, 6,  "3"}},
                          #else
                              {{-10.5, 5.41406, 10, "test"}, {10.5, 5.41406, 6, "3"}},
                          #endif
                                       {-TEST_LABEL_WIDTH/2, -ATOM_FONT_HEIGHT/2, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB});
  }

  void testSimpleAtomLabelWithSubscriptMiddle() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("te3st",
                          #ifdef MSK_QT5
                              {{-5.5, SVG_Y_ORIGIN, 10,  "te"}, {5.5, 5.5, 6,  "3"}, {9.5, SVG_Y_ORIGIN, 10,  "st"}},
                              {-5.5, -ATOM_FONT_HEIGHT/2, 25, ATOM_FONT_HEIGHT_WITH_SUB}
                          #else
                              {{-5.5, SVG_Y_ORIGIN, 10,  "te"}, {5.5, 5.41406, 6,  "3"}, {9.5, SVG_Y_ORIGIN, 10,  "st"}},
                              {-5.5, -ATOM_FONT_HEIGHT/2, 26, ATOM_FONT_HEIGHT_WITH_SUB}
                          #endif
                              );
  }

  void testSimpleAtomLabelWithSubscriptBeginning() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("3test",
                          #ifdef MSK_QT5
                              {{-10, 5.5, 10,  "test"}, {-14, 5.5, 6,  "3"}},
                              {-14, -SVG_Y_SUBSCRIPT, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB}
                          #else
                              {{-10.5, 5.41406, 10,  "test"}, {-14.5, 5.41406, 6,  "3"}},
                              {-14.5, -SVG_Y_SUBSCRIPT, TEST3_LABEL_WIDTH, ATOM_FONT_HEIGHT_WITH_SUB}
                          #endif
                              );
  }

  void testAtomWithTrailingHLeft() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("AB",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {-15.5, 5.5, 10,  "H"}},
                              {-15.5, -9.5, 30, 19},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 10,  "B"}, {-15.5, 5.41406, 10,  "H"}},
                              {-15.5, -9.3515625, 30, 18.703125},
                          #endif
                              Alignment::Left, 1);
  }

  void testAtomWithTrailingHRight() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("AB",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {14.5, 5.5, 10,  "H"}},
                              {-5.5, -9.5, 30, 19},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 10,  "B"}, {14.5, 5.41406, 10,  "H"}},
                              {-5.5, -9.3515625, 30, 18.703125},
                          #endif
                              Alignment::Right, 1);
  }

  void testAtomWithTrailingHDown() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("AB",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {-5, 24.5, 10,  "H"}},
                              {-5.5, -9.5, 20, 38},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 10,  "B"}, {-5, 24.1172, 10,  "H"}},
                              {-5.5, -9.3515625, 20, 37.40625},
                          #endif
                              Alignment::Down, 1);
  }

  void testAtomWithTrailingHUp() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("AB",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "B"}, {-5, -13.5, 10,  "H"}},
                              {-5.5, -28.5, 20, 38},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 10,  "B"}, {-5, -13.2891, 10,  "H"}},
                              {-5.5, -28.0546875, 20, 37.40625},
                          #endif
                              Alignment::Up, 1);
  }

  void testWithCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}},
                              {-5.5, -10.1, 16, 19.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  "+"}},
                              {-5.5, -9.5203125, 16, 18.871875},
                          #endif
                              Alignment::Right, 0, 1);
  }

  void testWithMultipleCharges() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "2+"}},
                              {-5.5, -10.1, 20, 19.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  "2+"}},
                              {-5.5, -9.5203125, 20, 18.871875},
                          #endif
                              Alignment::Right, 0, 2);
  }

  void testWithNegativeCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  QChar(0x2212)}},
                              {-5.5, -10.1, 16, 19.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  QChar(0x2212)}},
                              {-5.5, -9.5203125, 16, 18.871875},
                          #endif
                              Alignment::Right, 0, -1);
  }

  void testWithMultipleNegativeCharges() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  QString::number(2) + QChar(0x2212)}},
                              {-5.5, -10.1, 20, 19.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  QString::number(2) + QChar(0x2212)}},
                              {-5.5, -9.5203125, 20, 18.871875},
                          #endif
                              Alignment::Right, 0, -2);
  }

  void testWithMultipleElementsAndCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("AcBe",
                          #ifdef MSK_QT5
                              {{-8, 5.5, 10,  "Ac"}, {8, 5.5, 10,  "Be"}, {24, 5.5, 6,  "+"}},
                              {-8, -10.1, 37, 19.6},
                          #else
                              {{-8, 5.41406, 10,  "Ac"}, {8, 5.41406, 10,  "Be"}, {25, 5.41406, 6,  "+"}},
                              {-8, -9.5203125, 38, 18.871875},
                          #endif
                              Alignment::Right, 0, 1);
  }

  void testWithHatomRightAndCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "H"}, {15.5, 5.5, 6,  "+"}},
                              {-5.5, -10.1, 26, 19.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 10,  "H"}, {15.5, 5.41406, 6,  "+"}},
                              {-5.5, -9.5203125, 26, 18.871875},
                          #endif
                              Alignment::Right, 1, 1);
  }


  void testWithHatomsRightAndCharges() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 10,  "H"}, {15.5, 5.5, 6,  "2", false}, {15.5, 5.5, 6,  "2+"}},
                              {-5.5, -10.1, 30, 21.2},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 10,  "H"}, {15.5, 5.41406, 6,  "2", false}, {15.5, 5.41406, 6,  "2+"}},
                              {-5.5, -9.5203125, 30, 19.74375},
                          #endif
                              Alignment::Right, 2, 2);
  }

  void testWithHatomLeftAndCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}, {-15.5, 5.5, 10,  "H"}},
                              {-15.5, -10.1, 26, 19.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  "+"}, {-15.5, 5.41406, 10,  "H"}},
                              {-15.5, -9.5203125, 26, 18.871875},
                          #endif
                              Alignment::Left, 1, 1);
  }

  void testWithHatomUpAndCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}, {-5, -13.5, 10,  "H"}},
                              {-5.5, -29.1, 16, 38.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  "+"}, {-5, -13.2891, 10,  "H"}},
                              {-5.5, -28.2234375, 16, 37.575},
                          #endif
                              Alignment::Up, 1, 1);
  } //TODO: why is the left coordinate of H not -5.5? Align on bottom left/top left corner instead?

  void testWithHatomDownAndCharge() { TS_SKIP("NEEDS FIXING!");
    assertLabelAndBoundingBox("A",
                          #ifdef MSK_QT5
                              {{-5.5, 5.5, 10,  "A"}, {5.5, 5.5, 6,  "+"}, {-5, 24.5, 10,  "H"}},
                              {-5.5, -10.1, 16, 38.6},
                          #else
                              {{-5.5, 5.41406, 10,  "A"}, {5.5, 5.41406, 6,  "+"}, {-5, 24.1172, 10,  "H"}},
                              {-5.5, -9.5203125, 16, 37.575},
                          #endif
                              Alignment::Down, 1, 1);
  }
};
