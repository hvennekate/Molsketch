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

#include "coreatom.h"

namespace Molsketch {
namespace Core {

Atom::Atom(const QString &element, const QPointF &position, unsigned hAtoms, int charge)
  : el(element), pos(position), hAtomCount(hAtoms), ch(charge)
{}

Atom::Atom(const QString &element, unsigned hAtoms, int charge, const QPointF &position)
  : Atom(element, position, hAtoms, charge)
{}

Atom::Atom(const Atom &other, const QPointF &newPosition)
  : Atom(other.element(), newPosition, other.hAtoms(), other.charge())
{}

QPointF Atom::position() const {
  return pos;
}

QString Atom::element() const {
  return el;
}

unsigned Atom::hAtoms() const {
  return hAtomCount;
}

int Atom::charge() const {
  return ch;
}

} // namespace Core
} // namespace Molsketch
