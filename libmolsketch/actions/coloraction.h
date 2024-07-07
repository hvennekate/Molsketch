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

/** @file
 * This file is part of molsKetch and contains the colorAction class.
 *
 * @author Hendrik Vennekate <Hendrik.Vennekate@posteo.de>
 * @since Lithium
 */

#ifndef COLORACTION_H
#define COLORACTION_H

#include "actions/abstractrecursiveitemaction.h"


namespace Molsketch {
  class graphicsItem ;

  class colorAction : public abstractRecursiveItemAction
  {
    Q_OBJECT
  public:
    explicit colorAction(MolScene *parent = 0);
    ~colorAction();
  private:
    void execute() override;
    class privateData;
    privateData *d;
  };
} // namespace

#endif // COLORACTION_H
