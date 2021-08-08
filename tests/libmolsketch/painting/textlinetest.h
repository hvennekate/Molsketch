#ifndef TEXTLINETEST_H
#define TEXTLINETEST_H

#include <QDebug>
#include <QPainter>
#include <QPicture>
#include <QRectF>
#include <cxxtest/TestSuite.h>
#include <painting/textline.h>
#include "utilities.h"
#include "testtextbox.h"
#include "painting/stackedtextbox.h"

using namespace Molsketch;

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
    QS_ASSERT_EQUALS(tl.boundingRect(), QRectF(-5.5, -6, 11, 12));
  }

  void testWithLeftBox() {
    auto center = new TestTextBox, left = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    left->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    TextLine tl(center);
    tl.addBoxLeft(left);
    QS_ASSERT_EQUALS(tl.boundingRect(), QRectF(-9.5, -6, 15, 12));
  }

  void testWithTwoLeftBoxes() {
    auto center = new TestTextBox, left1 = new TestTextBox, left2 = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    left1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    left2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextLine tl(center);
    tl.addBoxLeft(left1);
    tl.addBoxLeft(left2);
    QS_ASSERT_EQUALS(tl.boundingRect(), QRectF(-15.5, -6, 21, 12));
  }

  void testWithRightBox() {
    auto center = new TestTextBox, right = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    right->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    TextLine tl(center);
    tl.addBoxRight(right);
    QS_ASSERT_EQUALS(tl.boundingRect(), QRectF(-5.5, -6, 15, 12));
  }

  void testWithTwoRightBoxes() {
    auto center = new TestTextBox, right1 = new TestTextBox, right2 = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    right1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    right2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextLine tl(center);
    tl.addBoxRight(right1);
    tl.addBoxRight(right2);
    QS_ASSERT_EQUALS(tl.boundingRect(), QRectF(-5.5, -6, 21, 12));
  }

  void testWithTwoRightAndLeftBoxes() {
    auto center = new TestTextBox, right1 = new TestTextBox, right2 = new TestTextBox, left1 = new TestTextBox, left2 = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    right1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    right2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    left1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    left2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextLine tl(center);
    tl.addBoxLeft(left1);
    tl.addBoxLeft(left2);
    tl.addBoxRight(right1);
    tl.addBoxRight(right2);
    QS_ASSERT_EQUALS(tl.boundingRect(), QRectF(-15.5, -6, 31, 12));
  }

  void testRenderingWithTwoRightAndLeftBoxes() {
    auto center = new TestTextBox,
        right1 = new TestTextBox,
        right2 = new TestTextBox,
        left1 = new TestTextBox,
        left2 = new TestTextBox;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    left1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    left2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    right1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    right2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };

    QTransform translationCenter, translationRight1, translationRight2, translationLeft1, translationLeft2;

    center->storePainterTransform(translationCenter);
    right1->storePainterTransform(translationRight1);
    right2->storePainterTransform(translationRight2);
    left1->storePainterTransform(translationLeft1);
    left2->storePainterTransform(translationLeft2);

    TextLine tl(center);
    tl.addBoxLeft(left1);
    tl.addBoxLeft(left2);
    tl.addBoxRight(right1);
    tl.addBoxRight(right2);

    QPicture picture;
    QPainter painter(&picture);
    tl.paint(&painter);

    QS_ASSERT_EQUALS(translationCenter, QTransform::fromTranslate(-14.5, -16))
    QS_ASSERT_EQUALS(translationRight1, QTransform::fromTranslate(-3.5, -16))
    QS_ASSERT_EQUALS(translationRight2, QTransform::fromTranslate(0.5, -16))
    QS_ASSERT_EQUALS(translationLeft1, QTransform::fromTranslate(-27.5, -16))
    QS_ASSERT_EQUALS(translationLeft2, QTransform::fromTranslate(-42.5, -16))
  }
  // TODO: check boxes of differing heights
};

#endif // TEXTLINETEST_H
