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

#ifndef MOLSKETCH_TEXTLINE_H
#define MOLSKETCH_TEXTLINE_H

#include "paintableaggregate.h"

namespace Molsketch {

  class TextBox;

  class TextLine : public PaintableAggregate {
    QRectF addRectFBefore(const QRectF &base, QRectF toAdd) const override;
    QRectF addRectFAfter(const QRectF &base, QRectF toAdd) const override;
    QPointF beforeItemPreShift(const Paintable *item) const override;
    QPointF beforeItemPostShift(const Paintable *item) const override;
    QPointF afterItemPreShift(const Paintable *item) const override;
    QPointF afterItemPostShift(const Paintable *item) const override;
  public:
    explicit TextLine(const TextBox *centerBox);
    void addBoxRight(const TextBox *newBox);
    void addBoxLeft(const TextBox *newBox);
  };

} // namespace Molsketch

#endif // MOLSKETCH_TEXTLINE_H
