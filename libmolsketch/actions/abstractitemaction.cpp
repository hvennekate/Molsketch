/***************************************************************************
 *   Copyright (C) 2007 by Harm van Eersel                                 *
 *   devsciurus@xs4all.nl                                                  *
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
#include "abstractitemaction.h"

#include <QSet>
#include "molscene.h"

namespace Molsketch {

  struct AbstractItemAction::privateData
  {
    QSet<graphicsItem *> itemList ;
    AbstractItemAction *parent;
    int minItemCount;

    privateData(AbstractItemAction* p)
      : parent(p), minItemCount(0) {}
    void checkItems()
    {
      itemList.remove(0) ; // TODO this seems wrong!
      parent->setEnabled(itemList.size() >= minItemCount);
      emit parent->itemsChanged();
    }
  };

  QSet<graphicsItem *> AbstractItemAction::filterItems(const QList<QGraphicsItem *> &inputItems) const
  {
    QSet<graphicsItem*> result;
    foreach (QGraphicsItem* item, inputItems)
      result << dynamic_cast<graphicsItem*>(item);
    result.remove(0);
    return result;
  }


  AbstractItemAction::AbstractItemAction(MolScene *parent) :
    genericAction(parent),
    d(new privateData(this))
  {
    connect(this, SIGNAL(triggered()), this, SLOT(gotTrigger())) ;
    connect(parent, SIGNAL(selectionChanged()), this, SLOT(updateItems()));
  }

  AbstractItemAction::~AbstractItemAction()
  {
    delete d;
  }

  void AbstractItemAction::setItem(graphicsItem * item)
  {
    setItems(QList<QGraphicsItem*>() << item);
  }

  void AbstractItemAction::setItems(const QList<QGraphicsItem *>& list)
  {
    d->itemList = filterItems(list);
    d->checkItems();
  }

  void AbstractItemAction::removeItem(graphicsItem *item)
  {
    removeItems(QList<graphicsItem*>() << item) ;
  }

  void AbstractItemAction::removeItems(const QList<graphicsItem *> &list)
  {
    foreach(graphicsItem* item, list)
      d->itemList.remove(item) ;
    d->checkItems();
  }

  void AbstractItemAction::clearItems()
  {
    d->itemList.clear();
    d->checkItems();
  }

  void AbstractItemAction::addItem(graphicsItem *item)
  {
    d->itemList << item;
    d->checkItems();
  }

  void AbstractItemAction::setMinimumItemCount(const int &count)
  {
    d->minItemCount = count;
    d->checkItems();
  }

  QList<graphicsItem *> AbstractItemAction::items() const
  {
    return d->itemList.values();
  }

  void AbstractItemAction::gotTrigger()
  {
    if (d->itemList.size() < d->minItemCount) return ;
    execute();
  }

  void AbstractItemAction::updateItems()
  {
    if (!scene()) return;
    setItems(scene()->selectedItems());
  }

  QSet<graphicsItem *> TopLevelItemAction::filterItems(const QList<QGraphicsItem *> &inputItems) const {
    QSet<graphicsItem*> result;
    for (auto item : inputItems)
      if (!item->parentItem())
        result << dynamic_cast<graphicsItem*>(item);
    result.remove(nullptr);
    return result;
  }

} // namespace Molsketch
