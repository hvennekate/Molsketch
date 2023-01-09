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

#include "corebond.h"

#include <QDebug>

namespace Molsketch {
namespace Core {

  Bond::Type Bond::fromOrder(const unsigned &order) {
    switch (order)
    {
      case 1: return Single;
      case 2: return DoubleLegacy;
      case 3: return Triple;
      default: return Invalid;
    }
  }

  Bond::Bond(unsigned start, unsigned end, Type type)
  : m_start(start), m_end(end), m_type(type) {}

unsigned Bond::start() const {
  return m_start;
}

unsigned Bond::end() const {
  return m_end;
}

Bond::Type Bond::type() const {
  return m_type;
}

unsigned Bond::order() const {
  return m_type / 10;
}

} // namespace Core
} // namespace Molsketch
