/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate                               *
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
#ifndef LINEUPACTION_H
#define LINEUPACTION_H

#include <actions/abstractitemaction.h>

namespace Molsketch {

  class Molecule;

  class LineUpAction : public TopLevelItemAction {
    Q_OBJECT
  public:
    static LineUpAction* horizontal(MolScene *scene = 0);
    static LineUpAction* vertical(MolScene *scene = 0);
  private:
    void execute() override;
  protected:
    explicit LineUpAction(MolScene *scene);
    void spaceItemsEqually(qreal distance, bool distanceBetweenCenters);
    virtual qreal getOrderingValue(const graphicsItem*) const = 0;
    virtual QPointF offsetForEdges(const graphicsItem* reference, const graphicsItem *item, qreal distance) const = 0;
    virtual QPointF offsetForCenters(const graphicsItem* reference, const graphicsItem *item, qreal distance) const = 0;
  };

} // namespace Molsketch

#endif // LINEUPACTION_H
