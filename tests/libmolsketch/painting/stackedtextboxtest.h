#ifndef REGULARTEXTBOXTEST_H
#define REGULARTEXTBOXTEST_H

#include <cxxtest/TestSuite.h>
#include "painting/stackedtextbox.h"
#include <QDebug>
#include <QRectF>


using namespace Molsketch;
const QFont TEST_FONT;

class StackedTextBoxTest : public CxxTest::TestSuite {
public:
  void testBoundingBox() {
    // TODO remove offset
    auto testBox = StackedTextBox("T", "B", {0, 0}, TEST_FONT, QFontMetricsF(TEST_FONT));
    qDebug() << testBox.boundingRect();
  }
};


#endif // REGULARTEXTBOXTEST_H
