#ifndef REGULARTEXTBOXTEST_H
#define REGULARTEXTBOXTEST_H

#include <cxxtest/TestSuite.h>
#include "painting/regulartextbox.h"
#include "utilities.h"
#include <QDebug>
#include <QRectF>


using namespace Molsketch;
const QFont FONT;

class RegularTextBoxTest : public CxxTest::TestSuite {
public:
  void testBoundingBox() {
    // TODO remove offset
    auto testBox = RegularTextBox("T", {0,0}, FONT);
    auto boundingRectangle = testBox.boundingRect();
    QFontMetricsF metrics(FONT);
    auto expectedCenter = QPointF(0, - metrics.descent()/2.);
    qDebug() << boundingRectangle;
//    QS_ASSERT_EQUALS(boundingRectangle.center(), expectedCenter)
  }
};


#endif // REGULARTEXTBOXTEST_H
