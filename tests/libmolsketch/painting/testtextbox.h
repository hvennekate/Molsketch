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

#ifndef TESTTEXTBOX_H
#define TESTTEXTBOX_H

#include "mocks.h"
#include "painting/textbox.h"
#include "utilities.h"

using namespace Molsketch;

class TestTextBox : public TextBox, public ForTesting<TextBox> {
  QDebug debug(QDebug debug) const override;
public:
  TestTextBox() : TextBox(QFont()) {}
  MOCK_CONST(QRectF, boundingRect, , )
  VOID_MOCK_CONST(paint, QPainter* painter, painter)
  static std::function<void(const int&)> destructorCallback;
  void storePainterTransform(QTransform &transform);
  bool preferredCenter() const override;
};

#endif // TESTTEXTBOX_H
