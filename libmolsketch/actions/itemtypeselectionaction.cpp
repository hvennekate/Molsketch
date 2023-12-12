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
#include "itemtypeselectionaction.h"
#include "ui_typelistdialog.h"
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <arrow.h>
#include <atom.h>
#include <bond.h>
#include <frame.h>
#include <molecule.h>
#include <molscene.h>
#include "iconutils.h"

namespace Molsketch {

ItemTypeSelectionAction::ItemTypeSelectionAction(MolScene *parent)
  : AbstractItemAction(parent){
  setMinimumItemCount(0);
  setCheckable(false);
  setIcon(getInternalIcon("select-by-type"));
  setWhatsThis(tr("Select items by type within the current selection or the entire document if nothing is selected"));
  setToolTip(tr("Select by type"));
  setText(tr("Select by type..."));
  setObjectName("item-type-selection-action");
}

void ItemTypeSelectionAction::execute() {
  qInfo() << "Executing type selection action" << this;
  QDialog dialog;
  Ui::TypeListDialog ui;
  ui.setupUi(&dialog);
  if (QDialog::Accepted != dialog.exec()) return; // TODO test

  // TODO test this:
  //  - all types unique
  //  - type name matches name on checkbox (substring)
  QList<int> selectedTypes;
  if (ui.arrows->isChecked()) selectedTypes << Arrow::Type;
  if (ui.atoms->isChecked()) selectedTypes << Atom::Type;
  if (ui.bonds->isChecked()) selectedTypes << Bond::Type;
  if (ui.frames->isChecked()) selectedTypes << Frame::Type;
  if (ui.molecules->isChecked()) selectedTypes << Molecule::Type;

  if (!scene()) return;
  QList<QGraphicsItem*> items = scene()->selectedItems();
  qDebug() << "selected items:" << items;
  if (items.isEmpty()) items = scene()->items();
  qDebug() << "relevant items:" << items;
  selectItems(scene(), itemsByType(items, selectedTypes));
}

QList<QGraphicsItem *> ItemTypeSelectionAction::itemsByType(const QList<QGraphicsItem *>& items, const QList<int>& types) {
  QList<QGraphicsItem*> result;
  for (QGraphicsItem *item : items) {
    if (!item) continue;
    if (types.contains(item->type())) result << item;
    else result << itemsByType(item->childItems(), types);
  }
  return result;
}

void ItemTypeSelectionAction::selectItems(QGraphicsScene *scene, const QList<QGraphicsItem *> &items) {
  qInfo() << "setting new selection" << scene << items;
  scene->clearSelection();
  for(QGraphicsItem* item : items) {
    if (!item) continue;
    item->setSelected(true);
  }
}

} // namespace Molsketch
