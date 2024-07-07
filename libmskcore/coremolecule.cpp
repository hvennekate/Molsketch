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

#include "coremolecule.h"

#include "coreatom.h"
#include "corebond.h"

namespace Molsketch {
namespace Core {

Molecule::Molecule(std::vector<Atom> atoms, std::vector<Bond> bonds, const std::string &name)
    : m_atoms(atoms), m_bonds(bonds), m_name(name) {}

std::string Molecule::name() const {
    return m_name;
}

std::vector<Atom> Molecule::atoms() const {
    return m_atoms;
}

std::vector<Bond> Molecule::bonds() const {
    return m_bonds;
}

Position Molecule::center() const {
    return Position::center(coordinates());
}

Coordinates Molecule::coordinates() const {
    Coordinates coords;
    for (auto atom : atoms()) coords.push_back(atom.position());
    return coords;
}

Molecule Molecule::shiftedBy(const Position &shift) const {
    std::vector<Atom> shiftedAtoms;
    for (auto atom : atoms()) shiftedAtoms.push_back(Atom(atom, atom.position() + shift));
    return Molecule(shiftedAtoms, bonds(), name());
}

bool Molecule::isValid() const {
    return !atoms().empty();
}

} // namespace Core
} // namespace Molsketch

