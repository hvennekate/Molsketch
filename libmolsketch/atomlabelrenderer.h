/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
 *   Copyright (C) 2019 Hendrik Vennekate                                  *
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

#ifndef ATOMLABELRENDERER_H
#define ATOMLABELRENDERER_H

#include <QFont>
#include <QPair>
#include <QRectF>
#include <QString>
#include <QFontMetricsF>
#include <alignment.h>

#include "alignment.h"

class QPainter;

namespace Molsketch {
  class TextBox;
  class TextField;

  class AtomLabelRenderer {
  public:
    void drawAtomLabel(QPainter *painter, const QString &lbl, const QFont &font, int hAtomCount, Alignment alignment, int charge);
    QRectF computeBoundingRect(const QString &lbl, const QFont &font, int hAtomCount, Alignment alignment, int charge);
  private:
    TextField *generateTextBoxes(const QString &lbl, const QFont &font, Alignment alignment, int hAtomCount, int charge);
  };

} // namespace Molsketch

#endif // ATOMLABELRENDERER_H
