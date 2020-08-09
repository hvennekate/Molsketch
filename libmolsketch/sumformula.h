/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate                               *
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
#ifndef SUMFORMULA_H
#define SUMFORMULA_H

#include <QScopedPointer>
#include <QMap>

namespace Molsketch {

  class SumFormulaPrivate;

  class SumFormula
  {
    Q_DECLARE_PRIVATE(SumFormula)
    QScopedPointer<SumFormulaPrivate> d_ptr;
  public:
    SumFormula(const QString& atomSymbol, int count = 1, int charge = 0, int isotope = 0);
    SumFormula(const std::initializer_list<SumFormula> &items);
    SumFormula(const SumFormula& other);
    SumFormula();
    virtual ~SumFormula();
    operator QString() const;
    int charge() const;
    qreal calculateMass(const QMap<QString, qreal>& atomicMasses, const QMap<std::pair<QString, int>, qreal>& isotopeMasses = {}) const;
    SumFormula& operator+=(const SumFormula &other);
    SumFormula operator+(const SumFormula &other) const;
    bool operator ==(const SumFormula &other) const;
    bool operator !=(const SumFormula &other) const;
    QString toHtml() const;
    QString toString();
    static SumFormula fromString(const QString& formula, bool *success = nullptr);
  };

} // namespace Molsketch

#endif // SUMFORMULA_H
