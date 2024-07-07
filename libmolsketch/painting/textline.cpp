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

#include "textline.h"
#include "textbox.h"
#include <algorithm>
#include <QPainter>

namespace Molsketch {

  QRectF TextLine::addRectFBefore(const QRectF &base, QRectF toAdd) const {
    if (!toAdd.isValid()) return base;
    toAdd.moveRight(base.left());
    return toAdd | base;
  }

  QRectF TextLine::addRectFAfter(const QRectF &base, QRectF toAdd) const {
    if (!toAdd.isValid()) return base;
    toAdd.moveLeft(base.right());
    return base | toAdd;
  }

  QPointF TextLine::beforeItemPreShift(const Paintable *item) const {
    auto bounds = item->boundingRect();
    return QPointF(-bounds.left() - bounds.width(), 0);
  }

  QPointF TextLine::beforeItemPostShift(const Paintable *item) const {
    Q_UNUSED(item)
    return QPointF();
  }

  QPointF TextLine::afterItemPreShift(const Paintable *item) const {
    return QPointF(-item->boundingRect().left(), 0);
  }

  QPointF TextLine::afterItemPostShift(const Paintable *item) const {
    return QPointF(item->boundingRect().right(), 0);
  }

  TextLine::TextLine(const TextBox *centerBox)
    : PaintableAggregate(centerBox) {}

  void TextLine::addBoxRight(const TextBox *newBox) {
    addAfter(newBox);
  }

  void TextLine::addBoxLeft(const TextBox *newBox) {
    addBefore(newBox);
  }

} // namespace Molsketch
