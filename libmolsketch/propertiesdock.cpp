/***************************************************************************
 *   Copyright (C) 2018 Hendrik Vennekate                                  *
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
#include "molscene.h"
#include "propertiesdock.h"
#include <QScrollArea>
#include <graphicsitem.h>

namespace Molsketch {

  struct PropertiesDockPrivate {
    QScrollArea *scrollArea;
  };

  PropertiesDock::PropertiesDock(QWidget *parent)
    : QDockWidget(tr("Properties"), parent),
      d_ptr(new PropertiesDockPrivate)
  {
    Q_D(PropertiesDock);
    d->scrollArea = new QScrollArea(this);
    d->scrollArea->setWidgetResizable(true);
    setWidget(d->scrollArea);
  }

  PropertiesDock::~PropertiesDock() {}

  graphicsItem *getCurrentItem(QList<QGraphicsItem*> items) {
    if (1 != items.size()) return nullptr;
    return dynamic_cast<graphicsItem*>(items.first());
  }

  void PropertiesDock::selectionChanged() {
    Q_D(PropertiesDock);
    MolScene *scene = qobject_cast<MolScene*>(sender());
    if (!scene) return;
    auto selectedItems = scene->selectedItems();

    if (auto currentItem = getCurrentItem(selectedItems))
      d->scrollArea->setWidget(currentItem->getPropertiesWidget());
    else
      d->scrollArea->setWidget(scene->getPropertiesWidget());
  }

} // namespace Molsketch
