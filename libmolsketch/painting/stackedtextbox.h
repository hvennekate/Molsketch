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

#ifndef MOLSKETCH_STACKEDTEXTBOX_H
#define MOLSKETCH_STACKEDTEXTBOX_H

#include "textbox.h"
#include "debuggable.h"

namespace Molsketch {

class StackedTextBox : public TextBox {
  QString topText, bottomText;
  QFontMetricsF originalFontMetrics;
  qreal shiftUp, shiftDown;
  QDebug debug(QDebug debug) const override;
public:
  StackedTextBox(const QString &topText,
                 const QString &bottomText,
                 const QFont & font);
  void paint(QPainter *painter) const override;
  QRectF boundingRect() const override;
  bool preferredCenter() const override;
  QString getTopText() const;
  QString getBottomText() const;
};

} // namespace Molsketch

#endif // MOLSKETCH_STACKEDTEXTBOX_H
