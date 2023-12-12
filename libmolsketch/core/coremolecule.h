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

#ifndef MOLSKETCH_CORE_MOLECULE_H
#define MOLSKETCH_CORE_MOLECULE_H

#include <QString>
#include <QVector>
#include "coreatom.h"
#include "corebond.h"

namespace Molsketch {
namespace Core {

class Molecule
{
    QVector<Atom> m_atoms;
    QVector<Bond> m_bonds;
    QString m_name;
public:
    Molecule(QVector<Atom> atoms, QVector<Bond> bonds, const QString &name = "");
    QString name() const;
    QVector<Atom> atoms() const;
    QVector<Bond> bonds() const;
    // TODO unit tests
    QPointF center() const;
    QPolygonF coordinates() const;
    Molecule shiftedBy(const QPointF &shift) const;
    bool isValid() const;
};

} // namespace Core
} // namespace Molsketch

QDebug operator<<(QDebug debug, const Molsketch::Core::Molecule &attributes);

#endif // MOLSKETCH_CORE_MOLECULE_H
