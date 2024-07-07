/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate                               *
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
#include "zlevelaction.h"

#include <QInputDialog>
#include "commands.h"
#include "iconutils.h"

namespace Molsketch {

ZLevelAction::ZLevelAction(MolScene *parent)
  : abstractRecursiveItemAction(parent)
{
  setIcon(getInternalIcon("layerset"));
  setText(tr("Set relative level..."));
  setToolTip(tr("Set the level at which the item will be drawn\n(higher values above lower values)"));
  setWhatsThis(tr("Set the level at which the item will be drawn\n(higher values above lower values)"));
  setCheckable(false);
  setMinimumItemCount(1);
}

void ZLevelAction::execute() {
  bool ok = false;
  qreal newZLevel = QInputDialog::getDouble(0,
                                            tr("New level"),
                                            tr("Level (higher is drawn on top of lower):"),
                                            items().size() == 1
                                            ? items().first()->zValue() // TODO this does not work for molecules
                                            : 0,
                                            -100,
                                            100,
                                            0,
                                            &ok);
  if (!ok) return;

  ITERATEOVERITEMSMACRO("Change level", SetZValue, newZLevel)
}

} // namespace Molsketch
