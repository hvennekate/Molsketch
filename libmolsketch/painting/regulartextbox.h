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

#ifndef MOLSKETCH_REGULARTEXTBOX_H
#define MOLSKETCH_REGULARTEXTBOX_H

#include <QFont>
#include <QPointF>
#include <QString>
#include "textbox.h"
#include "debuggable.h"

namespace Molsketch {

class RegularTextBox : public TextBox {
  QString text;
  QDebug debug(QDebug debug) const override;
public:
  RegularTextBox(const QString &text, const QFont &font);

  QRectF boundingRect() const override;
  void paint(QPainter *painter) const override;
  bool preferredCenter() const override;
};

} // namespace Molsketch

#endif // MOLSKETCH_REGULARTEXTBOX_H
