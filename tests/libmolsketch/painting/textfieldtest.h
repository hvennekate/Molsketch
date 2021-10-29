#ifndef TEXTLINETEST_H
#define TEXTLINETEST_H

#include <QDebug>
#include <QPainter>
#include <QPicture>
#include <QRectF>
#include <cxxtest/TestSuite.h>
#include "painting/textfield.h"
#include "painting/textline.h"
#include "painting/regulartextbox.h"
#include "utilities.h"
#include "testtextbox.h"

using namespace Molsketch;

class TestTextLine: public TextLine, public ForTesting<TextLine> {
public:
  TestTextLine() : TextLine(new RegularTextBox(QString(), QFont())) {}
  MOCK_CONST(QRectF, boundingRect, , )
  VOID_MOCK_CONST(paint, QPainter* painter, painter)
  static std::function<void(const int&)> destructorCallback;
};

class TextFieldTest : public CxxTest::TestSuite {
public:

  void tearDown() override {
    TS_ASSERT(!TestTextLine::instanceCounter);
  }

  void testSingleLine() {
    auto center = new TestTextLine;
    TS_ASSERT_EQUALS(TestTextLine::instanceCounter, 1)
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    TextField tf(center);
    QS_ASSERT_EQUALS(tf.boundingRect(), QRectF(-5.5, -6, 11, 12));
  }

  void testWithLineAbove() {
    auto center = new TestTextLine, above = new TestTextLine;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    above->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    TextField tf(center);
    tf.addLineAbove(above);
    QS_ASSERT_EQUALS(tf.boundingRect(), QRectF(-5.5, -18, 11, 24));
  }

  void testWithTwoLinesAbove() {
    auto center = new TestTextLine, above1 = new TestTextLine, above2 = new TestTextLine;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    above1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    above2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextField tf(center);
    tf.addLineAbove(above1);
    tf.addLineAbove(above2);
    QS_ASSERT_EQUALS(tf.boundingRect(), QRectF(-5.5, -30, 11, 36));
  }

  void testWithBoxBelow() {
    auto center = new TestTextLine, below = new TestTextLine;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    below->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    TextField tf(center);
    tf.addLineBelow(below);
    QS_ASSERT_EQUALS(tf.boundingRect(), QRectF(-5.5, -6, 11, 24));
  }

  void testWithTwoBoxesBelow() {
    auto center = new TestTextLine, below1 = new TestTextLine, below2 = new TestTextLine;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    below1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    below2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextField tf(center);
    tf.addLineBelow(below1);
    tf.addLineBelow(below2);
    QS_ASSERT_EQUALS(tf.boundingRect(), QRectF(-5.5, -6, 11, 36));
  }

  void testWithTwoBoxesBelowAndAbove() {
    auto center = new TestTextLine, below1 = new TestTextLine, below2 = new TestTextLine, above1 = new TestTextLine, above2 = new TestTextLine;
    center->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    below1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    below2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    above1->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    above2->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    TextField tf(center);
    tf.addLineAbove(above1);
    tf.addLineAbove(above2);
    tf.addLineBelow(below1);
    tf.addLineBelow(below2);
    QS_ASSERT_EQUALS(tf.boundingRect(), QRectF(-5.5, -30, 11, 60));
  }

  void testRenderingWithTwoBelowAndAboveBoxes() {
    auto centerLeft = new TestTextBox,
        centerMain = new TestTextBox,
        below1Main = new TestTextBox,
        below2Main = new TestTextBox,
        above1Main = new TestTextBox,
        above2Main = new TestTextBox;
    auto center = new TextLine(centerMain),
        below1 = new TextLine(below1Main),
        below2 = new TextLine(below2Main),
        above1 = new TextLine(above1Main),
        above2 = new TextLine(above2Main);
    center->addBoxLeft(centerLeft);

    centerLeft->boundingRectCallback = []() {return QRectF(9, 10, 11, 12); };
    centerMain->boundingRectCallback = []() {return QRectF(9, 10, 5, 12); };
    above1Main->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    above2Main->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };
    below1Main->boundingRectCallback = []() {return QRectF(9, 10, 4, 12); };
    below2Main->boundingRectCallback = []() {return QRectF(9, 10, 6, 12); };

    QTransform translationCenter, translationCenterLeft, translationBelow1, translationBelow2, translationAbove1, translationAbove2;

    centerMain->storePainterTransform(translationCenter);
    centerLeft->storePainterTransform(translationCenterLeft);
    below1Main->storePainterTransform(translationBelow1);
    below2Main->storePainterTransform(translationBelow2);
    above1Main->storePainterTransform(translationAbove1);
    above2Main->storePainterTransform(translationAbove2);

    TextField tf(center);
    tf.addLineAbove(above1);
    tf.addLineAbove(above2);
    tf.addLineBelow(below1);
    tf.addLineBelow(below2);

    QPicture picture;
    QPainter painter(&picture);
    tf.paint(&painter);

    QS_ASSERT_EQUALS(translationCenter, QTransform::fromTranslate(-11.5, -16))
    QS_ASSERT_EQUALS(translationCenterLeft, QTransform::fromTranslate(-31.5, -16))
    QS_ASSERT_EQUALS(translationBelow1, QTransform::fromTranslate(-11, -4))
    QS_ASSERT_EQUALS(translationBelow2, QTransform::fromTranslate(-12, 8))
    QS_ASSERT_EQUALS(translationAbove1, QTransform::fromTranslate(-11, -28))
    QS_ASSERT_EQUALS(translationAbove2, QTransform::fromTranslate(-12, -40))
  }
};

#endif // TEXTLINETEST_H
