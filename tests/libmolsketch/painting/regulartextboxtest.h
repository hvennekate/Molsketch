#ifndef REGULARTEXTBOXTEST_H
#define REGULARTEXTBOXTEST_H

#include <cxxtest/TestSuite.h>
#include "painting/regulartextbox.h"
#include "utilities.h"
#include <QDebug>
#include <QPainter>
#include <QPicture>
#include <QRectF>


using namespace Molsketch;
const QFont FONT("Courier", 10);

class RegularTextBoxTest : public CxxTest::TestSuite {
  RegularTextBox *box;
public:
  void setUp() override {
    box = new RegularTextBox("Tp", {0, 0}, FONT);
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


#endif // REGULARTEXTBOXTEST_H
