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
#include "obabelifaceloader.h"
#include "optimizestructureaction.h"
#include <commands.h>
#include <QDebug>
#include "scenesettings.h"
#include "settingsitem.h"
#include <algorithm>

OptimizeStructureAction::OptimizeStructureAction(OBabelIfaceLoader *obabelLoader, Molsketch::MolScene *scene)
  : FilteredItemAction<Molsketch::Molecule>(scene),
    obabelLoader(obabelLoader)
{
  setCheckable(false);
  setMinimumItemCount(1);
  setIcon(QIcon(":images/clean-structure.svg"));
  connect(obabelLoader, SIGNAL(optimizeAvailable(bool)), this, SLOT(setVisible(bool)));
}

void OptimizeStructureAction::execute() {
  using Molsketch::Molecule;
  if (items().isEmpty()) return;
  if (!obabelLoader) return;
  attemptBeginMacro(tr("Optimize structures"));
  for (auto item : items()) // TODO make items() for filtered covariant
    if (Molecule *molecule = dynamic_cast<Molecule*>(item))
    {
      QPolygonF newCoords = obabelLoader->optimizeCoordinates(molecule);
      auto bondLength = scene()->settings()->bondLength()->get();
      auto center = newCoords.boundingRect().center();
      newCoords.translate(-center);
      std::for_each(newCoords.begin(), newCoords.end(), [&](QPointF& point) { point *= bondLength; });
      newCoords.translate(center);
      qDebug() << "old molecule coords:" << molecule->coordinates()
               << "new molecule coords:" << newCoords;
      attemptUndoPush(new Molsketch::Commands::SetCoordinateCommand(item, newCoords));
    }
  attemptEndMacro();
}
