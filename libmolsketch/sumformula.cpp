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
#include "sumformula.h"

#include <QMap>
#include <QDebug>
#include <QRegularExpression>

namespace Molsketch {

  class ElementSymbol : public QString {
  public:
    ElementSymbol(const QString& other) : QString(other) {}
    bool operator<(const ElementSymbol &other) const {
      if (*this == other) return false;
      if (*this == "C") return true;
      if (other == "C") return false;
      if (*this == "H") return true;
      if (other == "H") return false;
      return (QString) *this < (QString) other; // TODO test
    }
  };
}

namespace Molsketch {
  struct SumFormulaPrivate {
    QMap<ElementSymbol, int> elementCounts;
    int charge;
    SumFormulaPrivate() : charge(0) {}
    QString format(const QString &subscriptOpen = "", const QString &subscriptClose = "",
                   const QString &superscriptOpen = "", const QString &superscriptClose = "",
                   bool trailingChargeSign = false) const {
      QString result;
      for (auto element : elementCounts.keys()) {
        int count = elementCounts[element];
        if (count != 1) result += element + subscriptOpen + QString::number(count) + subscriptClose;
        else result += element;
      }
      if (charge) {
        result += superscriptOpen;
        if (!trailingChargeSign) result += charge > 0 ? "+" : "-";
        if (!trailingChargeSign || qAbs(charge) != 1) result += QString::number(qAbs(charge));
        if (trailingChargeSign) result += charge > 0 ? "+" : "-";
        result += superscriptClose;
      }
      return result;
    }
  };

  SumFormula::SumFormula(const QString& atomSymbol, int count, int charge)
    : SumFormula()
  {
    Q_D(SumFormula);
    if (count <= 0) {
      qWarning() << "Tried to initialize sum formula with invalid element count. Element:" << atomSymbol << "count:" << count;
      return;
    }
    d->elementCounts[atomSymbol] = count;
    d->charge = charge;
  }

  SumFormula::SumFormula(const std::initializer_list<SumFormula> &items)
    : SumFormula()
  {
    for (auto item : items)
      *this += item;
  }

  SumFormula::SumFormula(const SumFormula &other)
    : SumFormula()
  {
    Q_D(SumFormula);
    d->elementCounts = other.d_ptr->elementCounts;
    d->charge = other.d_ptr->charge;
  }

  SumFormula::SumFormula()
    : d_ptr(new SumFormulaPrivate)
  {}

  SumFormula::~SumFormula() {}

  int SumFormula::charge() const {
    Q_D(const SumFormula);
    return d->charge;

  }

  SumFormula &SumFormula::operator+=(const SumFormula &other) {
    Q_D(SumFormula);
    for (auto element: other.d_ptr->elementCounts.keys())
      d->elementCounts[element] += other.d_ptr->elementCounts[element];
    d->charge += other.d_ptr->charge;
    return *this;
  }

  SumFormula SumFormula::operator+(const SumFormula &other) const {
    return SumFormula(*this) += other;
  }

  bool SumFormula::operator ==(const SumFormula &other) const  {
    Q_D(const SumFormula);
    return d->elementCounts == other.d_ptr->elementCounts
        && d->charge == other.d_ptr->charge;
  }

  bool SumFormula::operator !=(const SumFormula &other) const {
    return !(*this == other);
  }

  QString SumFormula::toHtml() const {
    Q_D(const SumFormula);
    return d->format("<sub>", "</sub>", "<super>", "</super>", true);
  }

  QString SumFormula::toString() {
    return (QString) *this;
  }

const QRegularExpression &ATOM_SYMBOL_REGEX = QRegularExpression("(([A-Z][a-z]*)([0-9]*)([+-][0-9]+)?)");
const QRegularExpression &FORMULA_REGEX = QRegularExpression("^" + ATOM_SYMBOL_REGEX.pattern() + "+$");

  SumFormula SumFormula::fromString(const QString &formula, bool *success) {
    bool matches = FORMULA_REGEX.match(formula).hasMatch();
    if (success) *success = matches;
    if (!matches) return SumFormula();

    auto result = SumFormula();
    auto atomSymbolIterator = ATOM_SYMBOL_REGEX.globalMatch(formula);
    while (atomSymbolIterator.hasNext()) {
      auto atomSymbolMatch = atomSymbolIterator.next();
      auto atomSymbol = atomSymbolMatch.captured(2);
      auto number = atomSymbolMatch.captured(3);
      int count = number.isEmpty() ? 1 : number.toInt();
      int charge = atomSymbolMatch.captured(4).toInt();
      result += SumFormula{atomSymbol, count, charge};
    }
    return result;
  }

  Molsketch::SumFormula::operator QString() const {
    Q_D(const SumFormula);
    return d->format();
  }

} // namespace Molsketch
