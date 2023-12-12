/***************************************************************************
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
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
#include "deleteaction.h"
#include "molscene.h"
#include "commands.h"
#include "iconutils.h"
#include <QDebug>
#include <molecule.h>

namespace Molsketch {

  deleteAction::deleteAction(MolScene *scene)
    : genericAction(scene)
  {
    setCheckable(false);
    connect(this, SIGNAL(triggered()), this, SLOT(deleteSelection()));
    setText(tr("Delete"));
    setIcon(getInternalIcon("delete"));
    setShortcut(QKeySequence::Delete);
  }

  deleteAction::~deleteAction()
  {
  }

  void deleteAction::deleteSelection()
  {
    if (!scene()) return;
    QList<QGraphicsItem*> selectedItems(scene()->selectedItems());
    if (selectedItems.isEmpty()) return;
    attemptBeginMacro(tr("Delete items"));

    for(auto item : selectedItems)
      if (Bond *bond = dynamic_cast<Bond*>(item))
        Commands::removeItemFromMolecule(bond, scene());

    for (auto item : selectedItems) {
      if (Atom *atom = dynamic_cast<Atom*>(item)) {
        for (auto bond : atom->bonds())
          Commands::removeItemFromMolecule(bond, scene());
        Commands::removeItemFromMolecule(atom, scene());
      }
    }

    for (auto item : selectedItems)
      if (!dynamic_cast<Atom*>(item) && !dynamic_cast<Bond*>(item))
        Commands::ItemAction::removeItemFromScene(item);

    for (auto item : scene()->items()) {
      if (Molecule *molecule = dynamic_cast<Molecule*>(item)) {
        if (!molecule->canSplit()) continue;
        foreach(Molecule* subMolecule, molecule->split())
          Commands::ItemAction::addItemToScene(subMolecule, scene());
        Commands::ItemAction::removeItemFromScene(molecule);
      }
    }
    attemptEndMacro();
  }

} // namespace Molsketch

