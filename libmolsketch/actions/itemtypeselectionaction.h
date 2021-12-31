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
#ifndef MOLSKETCH_ITEMTYPESELECTIONACTION_H
#define MOLSKETCH_ITEMTYPESELECTIONACTION_H

#include "abstractitemaction.h"

class QGraphicsScene;

namespace Molsketch {

class ItemTypeSelectionAction : public AbstractItemAction {
  Q_OBJECT
public:
    ItemTypeSelectionAction(MolScene *parent);
    void execute() override;
    static QList<QGraphicsItem*> itemsByType(const QList<QGraphicsItem *>& items, const QList<int>& types);
    static void selectItems(QGraphicsScene* scene, const QList<QGraphicsItem*>& items);
};
} // namespace Molsketch

#endif // MOLSKETCH_ITEMTYPESELECTIONACTION_H
