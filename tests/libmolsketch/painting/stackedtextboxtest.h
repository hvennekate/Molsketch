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
    box = new StackedTextBox("T", "B", FONT);
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
