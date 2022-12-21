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

#ifndef MOLSKETCH_TEXTBOX_H
#define MOLSKETCH_TEXTBOX_H

#include "paintable.h"

#include <QFont>
#include <QFontMetricsF>
#include <QPointF>


namespace Molsketch {

  class TextBox : public Paintable {
  public:
    virtual bool preferredCenter() const = 0;
  protected:
    QFont font;
    QFontMetricsF metrics;
    TextBox(const QFont &font) : font(font), metrics(font) {}
    // TODO: anchor points
  };


} // namespace Molsketch

#endif // MOLSKETCH_TEXTBOX_H
