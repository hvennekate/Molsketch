/***************************************************************************
 *   Copyright (C) 2022 by Hendrik Vennekate                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

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
#include <painting/textfield.h>
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
        (new RegularTextBox(text, TEST_FONT), &TextBox::paint, &TextBox::boundingRect);
  }

  DrawingTestDialog::Drawable *createStackedTextBox(const QString &upper, const QString &lower) {
    return new DelegatingDrawable<StackedTextBox>
        (new StackedTextBox(upper, lower, TEST_FONT), &TextBox::paint, &TextBox::boundingRect);
  }
public:
  void testRegularBoxDrawing() {
    QS_MANUAL_TEST
    DrawingTestDialog dialog(createRegularTextBox("C"));
    dialog.exec();
  }

  void testStackedTextBoxDrawing() {
    QS_MANUAL_TEST
    DrawingTestDialog dialog(createStackedTextBox("TOP", "BOTTOM"));
    dialog.exec();
  }

  void testRegularAndStackedBoxDrawing() {
    QS_MANUAL_TEST
    DrawingTestDialog dialog({
                               createStackedTextBox("ABC", "DEF"),
                               createRegularTextBox("XXX")
                             });
    dialog.exec();
  }

  void testStackedAndRegularInLine() {
    QS_MANUAL_TEST
    auto line = new TextLine(new RegularTextBox("C", TEST_FONT));
    line->addBoxLeft(new StackedTextBox("1", "3", TEST_FONT));
    line->addBoxLeft(new RegularTextBox("H", TEST_FONT));
    auto field = new TextField(line);
    DrawingTestDialog dialog(new DelegatingDrawable<TextField>(field, &TextField::paint, &TextField::boundingRect));
    dialog.exec();
  }

  void testRegularAndStackedInLine() {
    QS_MANUAL_TEST
    auto line = new TextLine(new RegularTextBox("C", TEST_FONT));
    line->addBoxRight(new RegularTextBox("H", TEST_FONT));
    line->addBoxRight(new StackedTextBox("3+", "3", TEST_FONT));
    auto field = new TextField(line);
    DrawingTestDialog dialog(new DelegatingDrawable<TextField>(field, &TextField::paint, &TextField::boundingRect));
    dialog.exec();
  }

  void testTextLine() {
    QS_MANUAL_TEST
    auto line = new TextLine(new RegularTextBox("XXX", TEST_FONT));
    line->addBoxLeft(new RegularTextBox("L", TEST_FONT));
    line->addBoxLeft(new RegularTextBox("K", TEST_FONT));
    line->addBoxRight(new RegularTextBox("R", TEST_FONT));
    line->addBoxRight(new RegularTextBox("S", TEST_FONT));
    DrawingTestDialog dialog(new DelegatingDrawable<TextLine>(line, &TextLine::paint, &TextLine::boundingRect));
    dialog.exec();
  }

  // TODO test non-centered lines (with box before/after center box)
  void testTextField() {
    QS_MANUAL_TEST
    auto field = new TextField(new TextLine(new RegularTextBox("XXX", TEST_FONT)));
    field->addLineAbove(new TextLine(new RegularTextBox("A", TEST_FONT)));
    field->addLineAbove(new TextLine(new RegularTextBox("Z", TEST_FONT)));
    field->addLineBelow(new TextLine(new RegularTextBox("B", TEST_FONT)));
    field->addLineBelow(new TextLine(new RegularTextBox("C", TEST_FONT)));
    DrawingTestDialog dialog(new DelegatingDrawable<TextField>(field, &TextField::paint, &TextField::boundingRect));
    dialog.exec();
  }
};

#endif // BOXDRAWINGMANUALTEST_H
