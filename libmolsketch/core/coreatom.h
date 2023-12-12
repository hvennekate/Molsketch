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

#include <QString>
#include <QPointF>
#include <QScopedPointer>

namespace Molsketch {
namespace Core {

class Atom
{
  QString el;
  QPointF pos;
  unsigned hAtomCount;
  int ch;
public:
  Atom(const QString &element, const QPointF &position = QPointF(), unsigned hAtoms = 0, int charge = 0);
  Atom(const QString &element, unsigned hAtoms, int charge = 0, const QPointF &position = QPointF());
  Atom(const Atom &other, const QPointF &newPosition);
  QPointF position() const;
  QString element() const;
  unsigned hAtoms() const;
  int charge() const;
};

} // namespace Core
} // namespace Molsketch

#endif // MOLSKETCH_CORE_ATOM_H
