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
#ifndef OPTIMIZESTRUCTUREACTION_H
#define OPTIMIZESTRUCTUREACTION_H

#include <actions/abstractitemaction.h>
#include <molecule.h>

class OBabelIfaceLoader;

class OptimizeStructureAction : public Molsketch::FilteredItemAction<Molsketch::Molecule> {
  Q_OBJECT
public:
  explicit OptimizeStructureAction(OBabelIfaceLoader *obabelLoader, Molsketch::MolScene *scene = 0);
private:
  void execute() override;
  OBabelIfaceLoader *obabelLoader;
};

#endif // OPTIMIZESTRUCTUREACTION_H
