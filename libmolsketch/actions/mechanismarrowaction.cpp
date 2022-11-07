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
#include "mechanismarrowaction.h"
#include "molscene.h"
#include "iconutils.h"

namespace Molsketch {

  mechanismArrowAction::mechanismArrowAction(MolScene* scene)
    : reactionArrowAction(scene)
  {
    // TODO make arrow action with all arrow tips available
    // TODO arrow action manipulates selected arrow items
    setText(tr("Curved arrow"));
    foreach(QAction* action, findChildren<QAction*>())
    {
      if (action->text() == tr("Single arrow"))
      {
        action->setIcon(getInternalIcon("mechanismarrow"));
        action->trigger();
      }
      if (action->text() == tr("Double arrow"))
        action->setIcon(getInternalIcon("curveddoublearrow"));
      if (action->text() == tr("Half arrow"))
        action->setIcon(getInternalIcon("curvedhalfarrow"));
    }
    setChecked(false);
  }

  mechanismArrowAction::~mechanismArrowAction()
  {
  }

  QPolygonF mechanismArrowAction::makePolygon(const QLineF &line)
  {
    QLineF normalVector = line.normalVector();
    normalVector.setLength(normalVector.length()/2);
    return QPolygonF() << scene()->snapToGrid(line.p1())
                       << scene()->snapToGrid(normalVector.p2())
                       << scene()->snapToGrid(normalVector.translated(line.p2()-line.p1()).p2())
                       << scene()->snapToGrid(line.p2());
  }

} // namespace Molsketch

