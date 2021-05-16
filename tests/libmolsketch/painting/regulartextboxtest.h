#ifndef REGULARTEXTBOXTEST_H
#define REGULARTEXTBOXTEST_H

#include <cxxtest/TestSuite.h>
#include "painting/regulartextbox.h"
#include <QDebug>
#include <QRectF>


using namespace Molsketch;
const QFont FONT;

class RegularTextBoxTest : public CxxTest::TestSuite {
public:
  void testBoundingBox() {
    auto testBox = RegularTextBox("T", {0,0}, FONT);
  }
};


#endif // REGULARTEXTBOXTEST_H
