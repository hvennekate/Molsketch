#ifndef TEXTLINETEST_H
#define TEXTLINETEST_H

#include <QDebug>
#include <QRectF>
#include <cxxtest/TestSuite.h>
#include <painting/textline.h>
#include "utilities.h"
#include "mocks.h"
#include "painting/stackedtextbox.h"

using namespace Molsketch;
const QFont TEST_FONT;

class TestTextBox : public TextBox, public ForTesting<TextBox> {
public:
  TestTextBox() : TextBox(QPointF(), QFont()) {}
  MOCK_CONST(QRectF, boundingRect, , )
  VOID_MOCK_CONST(render, QPainter* painter, painter)
  static std::function<void(const int&)> destructorCallback;
};


class TextLineTest : public CxxTest::TestSuite {
public:

  void tearDown() override {
    TS_ASSERT(!TestTextBox::instanceCounter);
  }

  void testSingleBox() {
    auto center = new TestTextBox;
    TS_ASSERT_EQUALS(TestTextBox::instanceCounter, 1)
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    TextLine tl(center);
    QS_ASSERT_EQUALS(tl.boundingBox(), QRectF(-5.5, -6, 11, 12));
  }

  void testWithLeftBox() {
    auto center = new TestTextBox, left = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    left->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    TextLine tl(center);
    tl.addBoxLeft(left);
    QS_ASSERT_EQUALS(tl.boundingBox(), QRectF(-9.5, -6, 15, 12));
  }

  void testWithTwoLeftBoxes() {
    auto center = new TestTextBox, left1 = new TestTextBox, left2 = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    left1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    left2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextLine tl(center);
    tl.addBoxLeft(left1);
    tl.addBoxLeft(left2);
    QS_ASSERT_EQUALS(tl.boundingBox(), QRectF(-15.5, -6, 21, 12));
  }

  void testWithRightBox() {
    auto center = new TestTextBox, right = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    right->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    TextLine tl(center);
    tl.addBoxRight(right);
    QS_ASSERT_EQUALS(tl.boundingBox(), QRectF(-5.5, -6, 15, 12));
  }

  void testWithTwoRightBoxes() {
    auto center = new TestTextBox, right1 = new TestTextBox, right2 = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    right1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    right2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextLine tl(center);
    tl.addBoxRight(right1);
    tl.addBoxRight(right2);
    QS_ASSERT_EQUALS(tl.boundingBox(), QRectF(-5.5, -6, 21, 12));
  }

  // TODO: test rendering (assert transform is applied)
  // TODO: check boxes of differing heights
};

#endif // TEXTLINETEST_H
