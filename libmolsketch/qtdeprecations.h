/***************************************************************************
 *   Copyright (C) 2021 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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
#ifndef QTDEPRECATIONS_H
#define QTDEPRECATIONS_H

#include <QSet>
#include <QList>
#include <QLineF>

namespace Molsketch {

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
template<class T>
inline QSet<T> toSet(const QList<T>& list) { return QSet<T>::fromList(list); }
inline QLineF::IntersectType intersectionType(const QLineF& first, const QLineF& second, QPointF *intersectionPoint) {
  return first.intersect(second, intersectionPoint);
}
#else
template<class T>
inline QSet<T> toSet(const QList<T>& list) { return QSet<T>(list.cbegin(), list.cend()); }

inline QLineF::IntersectType intersectionType(const QLineF& first, const QLineF& second, QPointF *intersectionPoint) {
  return first.intersects(second, intersectionPoint);
}
#endif
} // namespace Molsketch


#endif // QTDEPRECATIONS_H
