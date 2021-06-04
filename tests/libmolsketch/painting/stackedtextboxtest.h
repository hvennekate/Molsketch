#ifndef STACKEDTEXTBOXTEST_H
#define STACKEDTEXTBOXTEST_H

#include <cxxtest/TestSuite.h>
#include "painting/stackedtextbox.h"
#include <QDebug>
#include <QRectF>


using namespace Molsketch;
const QFont TEST_FONT("Times", 12);

class StackedTextBoxTest : public CxxTest::TestSuite {
public:
  void testBoundingBox() {
    // TODO remove offset
    auto testBox = StackedTextBox("T", "B", {0, 0}, TEST_FONT);
    qDebug() << testBox.boundingRect();
  }

  void testFontSize() {
    auto oldMetrics = QFontMetricsF(TEST_FONT);
    qDebug() << TEST_FONT << oldMetrics.ascent() << oldMetrics.descent();
    auto newFont = QFont(TEST_FONT);
    newFont.setPointSizeF(newFont.pointSizeF()/2.2);
    auto newMetrics = QFontMetricsF(newFont);
    qDebug() << newFont << newMetrics.ascent() << newMetrics.descent();
  }
};

#endif // STACKEDTEXTBOXTEST_H
