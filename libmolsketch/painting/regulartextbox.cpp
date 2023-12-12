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

#include "regulartextbox.h"
#include <QPainter>
#include <QDebug>

namespace Molsketch {

  QDebug RegularTextBox::debug(QDebug debug) const {
    return debug << "Regular text box(" << text << ", " << font << ")";
  }

  RegularTextBox::RegularTextBox(const QString &text, const QFont &font)
    : TextBox(font), text(text) {}

  QRectF RegularTextBox::boundingRect() const {
    // TODO consider tightBoundingRect() (possibly selectable by user)
    return metrics.boundingRect(text);
  }

  void RegularTextBox::paint(QPainter *painter) const {
    painter->save();
    painter->setFont(font);
    painter->drawText(0, 0, text);
    painter->restore();
  }

  bool RegularTextBox::preferredCenter() const {
    return true;
  }

} // namespace Molsketch
