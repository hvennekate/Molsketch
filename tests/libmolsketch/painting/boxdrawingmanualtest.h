#ifndef BOXDRAWINGMANUALTEST_H
#define BOXDRAWINGMANUALTEST_H

#include "painting/stackedtextbox.h"
#include "utilities.h"
#include "cxxtest/TestSuite.h"
#include "drawingtestdialog.h"

#include <QDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPicture>
#include <QVBoxLayout>

#include <painting/regulartextbox.h>

using namespace Molsketch;
const QFont TEST_FONT("Times", 12);

class TestStackedTextBox : public DrawingTestDialog::Drawable, private StackedTextBox {
public:
  TestStackedTextBox(const QString &upper, const QString &lower)
    : StackedTextBox(upper, lower, QPointF(), TEST_FONT) {}
  void draw(QPainter* painter) override {
    render(painter);
  }
  QRectF boundingRect() const override {
    return StackedTextBox::boundingRect();
  }
};

class TestRegularTextBox : public DrawingTestDialog::Drawable, private RegularTextBox {
public:
  TestRegularTextBox(const QString &text)
    : RegularTextBox(text, QPointF(), TEST_FONT) {}
  void draw(QPainter* painter) override {
    render(painter);
  }
  QRectF boundingRect() const override {
    return RegularTextBox::boundingRect();
  }
};

class BoxDrawingManualTest : public CxxTest::TestSuite {
public:
  void testSimpleBoxDrawing() {
//    TS_SKIP("Manual test");
    DrawingTestDialog dialog({
                               new TestStackedTextBox("ABC", "DEF"),
                               new TestRegularTextBox("XXX")
                             });
    dialog.exec();
  }
};

#endif // BOXDRAWINGMANUALTEST_H
