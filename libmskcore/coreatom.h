/***************************************************************************
 *   Copyright (C) 2022 by Hendrik Vennekate                               *
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

#ifndef MOLSKETCH_CORE_ATOM_H
#define MOLSKETCH_CORE_ATOM_H

#include <string>
#include "position.h"

namespace Molsketch {
namespace Core {

class Atom
{
  std::string el;
  Position pos;
  unsigned hAtomCount;
  int ch;
public:
  Atom(const std::string &element,
       const Position &position = QPointF(),
       unsigned hAtoms = 0,
       int charge = 0);
  Atom(const std::string &element,
       unsigned hAtoms,
       int charge = 0,
       const Position &position = QPointF());
  Atom(const Atom &other,
       const Position &newPosition);
  Position position() const;
  std::string element() const;
  unsigned hAtoms() const;
  int charge() const;
};

} // namespace Core
} // namespace Molsketch

#endif // MOLSKETCH_CORE_ATOM_H
