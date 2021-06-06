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
#include <painting/textline.h>

using namespace Molsketch;
const QFont TEST_FONT("Times", 12);

template<class T>
class DelegatingDrawable : public DrawingTestDialog::Drawable {
private:
  T *delegate;
  void (T::*drawMethod)(QPainter *) const;
  QRectF (T::*boundingRectMethod)() const;
public:
  DelegatingDrawable(T *delegate, void (T::*draw)(QPainter *) const, QRectF (T::*boundingRect)() const)
    : delegate(delegate), drawMethod(draw), boundingRectMethod(boundingRect) {}
  void draw(QPainter* painter) override {
    (delegate->*drawMethod)(painter);
  }
  QRectF boundingRect() const override {
    return (delegate->*boundingRectMethod)();
  }
  ~DelegatingDrawable() { delete delegate; }
};

class BoxDrawingManualTest : public CxxTest::TestSuite {
  DrawingTestDialog::Drawable *createRegularTextBox(const QString &text) {
    return new DelegatingDrawable<RegularTextBox>
        (new RegularTextBox(text, QPointF(), TEST_FONT), &TextBox::render, &TextBox::boundingRect);
  }

  DrawingTestDialog::Drawable *createStackedTextBox(const QString &upper, const QString &lower) {
    return new DelegatingDrawable<StackedTextBox>
        (new StackedTextBox(upper, lower, QPointF(), TEST_FONT), &TextBox::render, &TextBox::boundingRect);
  }
public:
  void testRegularBoxDrawing() {
    TS_SKIP("Manual test");
    DrawingTestDialog dialog(createRegularTextBox("C"));
    dialog.exec();
  }

  void testStackedTextBoxDrawing() {
    TS_SKIP("Manual test");
    DrawingTestDialog dialog(createStackedTextBox("TOP", "BOTTOM"));
    dialog.exec();
  }

  void testBothTextBoxDrawing() {
    TS_SKIP("Manual test");
    DrawingTestDialog dialog({
                               createStackedTextBox("ABC", "DEF"),
                               createRegularTextBox("XXX")
                             });
    dialog.exec();
  }

  void testTextLine() {
    TS_SKIP("Manual test");
    auto line = new TextLine(new RegularTextBox("XXX", QPointF(), TEST_FONT));
    line->addBoxLeft(new RegularTextBox("L", QPointF(), TEST_FONT));
    line->addBoxLeft(new RegularTextBox("K", QPointF(), TEST_FONT));
    line->addBoxRight(new RegularTextBox("R", QPointF(), TEST_FONT));
    line->addBoxRight(new RegularTextBox("S", QPointF(), TEST_FONT));
    DrawingTestDialog dialog(new DelegatingDrawable(line, &TextLine::render, &TextLine::boundingBox));
    dialog.exec();
  }
};

#endif // BOXDRAWINGMANUALTEST_H
