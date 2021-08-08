#ifndef STACKEDTEXTBOXTEST_H
#define STACKEDTEXTBOXTEST_H

#include <cxxtest/TestSuite.h>
#include "painting/stackedtextbox.h"
#include <QDebug>
#include <QPicture>
#include <QRectF>
#include <qpainter.h>
#include "utilities.h"


using namespace Molsketch;
const QFont FONT("Times", 12);

class StackedTextBoxTest : public CxxTest::TestSuite {
  StackedTextBox *box;
public:

  void setUp() override {
    box = new StackedTextBox("T", "B", {0, 0}, FONT);
  }

  void tearDown() override {
    delete box;
  }

  void testFontProperlyReset() {
    QPicture picture;
    QPainter painter(&picture);
    auto painterFont = painter.font();
    QS_ASSERT_NOT_EQUALS(painterFont.pointSize(), FONT.pointSize())
    box->paint(&painter);
    QS_ASSERT_EQUALS(painterFont.pointSize(), painter.font().pointSize())
  }

  void testAnchorPoints() {
    auto bounds = box->boundingRect();

    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::Center), bounds.center())

    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::TopLeft), bounds.topLeft())
    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::TopRight), bounds.topRight())
    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::BottomLeft), bounds.bottomLeft())
    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::BottomRight), bounds.bottomRight())

    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::Top), QPointF(bounds.center().x(), bounds.top()))
    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::Bottom), QPointF(bounds.center().x(), bounds.bottom()))
    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::Left), QPointF(bounds.left(), bounds.center().y()))
    QS_ASSERT_EQUALS(box->getAnchorPoint(Anchor::Right), QPointF(bounds.right(), bounds.center().y()))
  }
};

#endif // STACKEDTEXTBOXTEST_H
