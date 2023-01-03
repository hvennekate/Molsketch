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

#include <QPolygonF>

namespace Molsketch {
namespace Core {

Molecule::Molecule(QVector<Atom> atoms, QVector<Bond> bonds, const QString &name)
  : m_atoms(atoms), m_bonds(bonds), m_name(name) {}

QString Molecule::name() const {
  return m_name;
}

QVector<Atom> Molecule::atoms() const {
  return m_atoms;
}

QVector<Bond> Molecule::bonds() const {
  return m_bonds;
}

QPointF Molecule::center() const {
  return coordinates().boundingRect().center();
}

QPolygonF Molecule::coordinates() const {
  QPolygonF positions;
  for (auto atom : atoms()) positions << atom.position();
  return positions;
}

Molecule Molecule::shiftedBy(const QPointF &shift) const {
  QVector<Atom> shiftedAtoms;
  for (auto atom : atoms()) shiftedAtoms << Atom(atom, atom.position() + shift);
  return Molecule(shiftedAtoms, bonds(), name());
}

bool Molecule::isValid() const {
  return !atoms().empty();
}

} // namespace Core
} // namespace Molsketch

QDebug operator<<(QDebug debug, const Molsketch::Core::Molecule &molecule) {
  auto out = debug.nospace() << "Molecule[name=\"" << molecule.name() << "\", atoms=(";
  for (auto atom : molecule.atoms())
    out << "<" << atom.element() << ": " << atom.position() << ", " << atom.hAtoms() << ">";
  out <<"), bonds=(";
  for (auto bond: molecule.bonds())
    out << "<type=" << bond.type() << ", start=" << bond.start() << ", end=" << bond.end() << ">";
  out << ")]";
  return debug;
}
