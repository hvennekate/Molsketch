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

#ifndef MOLSKETCH_CORE_BOND_H
#define MOLSKETCH_CORE_BOND_H

#include <QDebug>

namespace Molsketch {
namespace Core {

class Bond
{
public:
  enum Type
  {
    Invalid = 0,
    DativeDot = 1,
    DativeDash = 2,
    Single = 10,
    Wedge = 11,
    Hash = 12,
    WedgeOrHash = 13,
    Thick = 14,
    Striped = 15,
    DoubleLegacy = 20,
    CisOrTrans = 21,
    DoubleAsymmetric = 22,
    DoubleSymmetric = 23,
    Triple = 30,
    TripleAsymmetric = 31, // TODO more?
  };
  static Type fromOrder(const unsigned &order);
private:
  unsigned m_start, m_end;
  Type m_type;
public:

  Bond(unsigned start, unsigned end, Type type = Single);
  unsigned start() const;
  unsigned end() const;
  Type type() const;
  unsigned order() const;
};

} // namespace Core
} // namespace Molsketch

#endif // MOLSKETCH_CORE_BOND_H
