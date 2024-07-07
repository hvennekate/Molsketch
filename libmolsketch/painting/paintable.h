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

#ifndef PAINTABLE_H
#define PAINTABLE_H

#include "boundingboxlinker.h"

#include <debuggable.h>

class QPainter;

namespace Molsketch {

  class Paintable : public Linkable, public Debuggable {
  public:
    virtual ~Paintable() {}
    virtual void paint(QPainter *painter) const = 0;
    virtual QRectF boundingRect() const = 0;
    QPointF getAnchorPoint(const Anchor &anchor) const override;
  };

} // namespace Molsketch

#endif // PAINTABLE_H
