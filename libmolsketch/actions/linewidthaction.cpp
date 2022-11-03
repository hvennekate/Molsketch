/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
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

#include <QInputDialog>
#include "linewidthaction.h"
#include "commands.h"
#include "iconutils.h"

namespace Molsketch {

  lineWidthAction::lineWidthAction(MolScene *parent) :
    abstractRecursiveItemAction(parent)
  {
    setText(tr("Text/line width scaling...")) ;
    setIcon(getInternalIcon("linewidth"));
    setToolTip(tr("Set line width")) ;
    setWhatsThis(tr("Set the relative line width for the selected item(s)"));
    setCheckable(false);
  }

  void lineWidthAction::execute()
  {
    bool ok = false ;
    qreal newFactor = QInputDialog::getDouble(0,
                                              tr("New line width"),
                                              tr("Relative line width:"),
                                              items().size() == 1
                                              ? items().first()->relativeWidth()
                                              : 1,
                                              0,
                                              2147483647,
                                              2,
                                              &ok) ;
    if (!ok) return ;

    ITERATEOVERITEMSMACRO("Change line width", changeRelativeWidth, newFactor)
  }

} // namespace
