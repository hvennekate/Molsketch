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

#include <cxxtest/TestSuite.h>
#include "utilities.h"
#include <sumformula.h>
#include <cmath>

using namespace Molsketch;

class SumFormulaUnitTest : public CxxTest::TestSuite {
public:
  void testCreation() {
    QS_ASSERT_EQUALS(SumFormula{"A"}.toString(), QString("A"));

    auto a2 = SumFormula{"A", 2};
    QS_ASSERT_EQUALS(a2.toString(), QString("A2"));

    auto ach = SumFormula{{"A"}, {"C"}, {"H"}};
    QS_ASSERT_EQUALS(ach.toString(), QString("CHA"));

    auto aa = SumFormula{{"A"}, {"H"}, {"A"}};
    QS_ASSERT_EQUALS(aa.toString(), QString("HA2"));

    auto a3minus = SumFormula{"A", 1, -3};
    QS_ASSERT_EQUALS(a3minus.charge(), -3);
    QS_ASSERT_EQUALS(a3minus.toString(), QString("A-3"));

    auto withEmptyElement = SumFormula{{"A"}, {"B", 0}};
    QS_ASSERT_EQUALS(withEmptyElement, SumFormula{"A"});

    auto withNegativeElement = SumFormula{{"A"}, {"B", -1}};
    QS_ASSERT_EQUALS(withNegativeElement, SumFormula{"A"});

    auto a3plus = SumFormula{"A", 1, +3};
    QS_ASSERT_EQUALS(a3plus.charge(), 3);
    QS_ASSERT_EQUALS(a3plus.toString(), QString("A+3"));

    auto ab2_3minus = SumFormula{{"A", 1, -1}, {"B", 2, -2}};
    QS_ASSERT_EQUALS(ab2_3minus.charge(), -3);
    QS_ASSERT_EQUALS(ab2_3minus.toString(), QString("AB2-3"));
  }

  void testFormatting() {
    QS_ASSERT_EQUALS(SumFormula{"A"}.toHtml(), QString("A"));

    auto a2 = SumFormula{"A", 20};
    QS_ASSERT_EQUALS(a2.toHtml(), QString("A<sub>20</sub>"));

    auto ach = SumFormula{{"A"}, {"C"}, {"H"}};
    QS_ASSERT_EQUALS(ach.toHtml(), QString("CHA"));

    auto aa = SumFormula{{"A"}, {"H"}, {"A"}};
    QS_ASSERT_EQUALS(aa.toHtml(), QString("HA<sub>2</sub>"));

    auto withSingleNegativeCharge = SumFormula{"A", 2, -1};
    QS_ASSERT_EQUALS(withSingleNegativeCharge.toHtml(), QString("A<sub>2</sub><super>-</super>"));

    auto withSinglePositiveCharge = SumFormula{"A", 2, 1};
    QS_ASSERT_EQUALS(withSinglePositiveCharge.toHtml(), QString("A<sub>2</sub><super>+</super>"));

    auto withNegativeCharge = SumFormula{"A", 2, -3};
    QS_ASSERT_EQUALS(withNegativeCharge.toHtml(), QString("A<sub>2</sub><super>3-</super>"));

    auto withPositiveCharge = SumFormula{"A", 2, 3};
    QS_ASSERT_EQUALS(withPositiveCharge.toHtml(), QString("A<sub>2</sub><super>3+</super>"));
  }

  void testAdding() {
    QS_ASSERT_EQUALS((SumFormula{"A"} + SumFormula{"B"}).toString(), QString("AB"));
    QS_ASSERT_EQUALS((SumFormula{"B"} + SumFormula{"A"}).toString(), QString("AB"));
    QS_ASSERT_EQUALS((SumFormula{"A"} + SumFormula{"A"}).toString(), QString("A2"));
    QS_ASSERT_EQUALS((SumFormula{"Ab"} + SumFormula{"A"}).toString(), QString("AAb"));

    auto a2 = SumFormula{"A", 2};
    QS_ASSERT_EQUALS((a2 + SumFormula{"A"}).toString(), QString("A3"));

    auto a1b = SumFormula{{"A", 1}, {"B"}};
    QS_ASSERT_EQUALS((a1b + SumFormula{"B"}).toString(), QString("AB2"));

    auto a1b2c3 = SumFormula{{"A", 1}, {"B", 2}, {"C", 3}};
    auto d3c5a = SumFormula{{"D", 3}, {"C", 5}, {"A"}};
    QS_ASSERT_EQUALS((a1b2c3 + d3c5a).toString(), QString("C8A2B2D3"));

    auto a3minus = SumFormula{"A", 1, -3};
    auto b2minus = SumFormula{"B", 1, -2};
    QS_ASSERT_EQUALS((a3minus + b2minus).toString(), QString("AB-5"));
  }

  void testEquality() {
    TS_ASSERT(SumFormula{"A"} == SumFormula("A"));
    TS_ASSERT(SumFormula{"B"} != SumFormula("A"));

    auto aa = SumFormula{{"A"}, {"A"}};
    auto a2 = SumFormula{{"A", 2}};
    TS_ASSERT(aa == a2);
    TS_ASSERT(!(aa != a2));

    auto abc = SumFormula{{"A"}, {"B"}, {"C"}};
    auto cba = SumFormula{{"C"}, {"B"}, {"A"}};
    TS_ASSERT(abc == cba);
    TS_ASSERT(!(abc != cba));

    auto ab = SumFormula{{"A"}, {"B"}};
    auto abElement = SumFormula{{"AB"}};
    TS_ASSERT(ab != abElement);
    TS_ASSERT(!(ab == abElement));

    auto a3minus = SumFormula{"A", 1, -3};
    TS_ASSERT(a3minus != SumFormula{"A"});
    TS_ASSERT(!(a3minus == SumFormula{"A"}));
    TS_ASSERT(a3minus == a3minus);
    TS_ASSERT(!(a3minus != a3minus));
  }

  void testParsingString() {
    QS_ASSERT_EQUALS(SumFormula::fromString("A"), SumFormula("A"));

    auto ab = SumFormula{{"A"}, {"B"}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AB"), ab);
    QS_ASSERT_EQUALS(SumFormula::fromString("BA"), ab);

    QS_ASSERT_EQUALS(SumFormula::fromString("Ab"), SumFormula("Ab"));

    auto aaa = SumFormula{{"A"}, {"Aa"}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AAa"), aaa);
    QS_ASSERT_EQUALS(SumFormula::fromString("AaA"), aaa);

    auto a2aa = SumFormula{{"A", 2}, {"Aa"}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AAAa"), a2aa);
    QS_ASSERT_EQUALS(SumFormula::fromString("A2Aa"), a2aa);
    QS_ASSERT_EQUALS(SumFormula::fromString("AAaA"), a2aa);
    QS_ASSERT_EQUALS(SumFormula::fromString("AaAA"), a2aa);
    QS_ASSERT_EQUALS(SumFormula::fromString("AaA2"), a2aa);

    auto aa2a = SumFormula{{"A"}, {"Aa", 2}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AaAaA"), aa2a);
    QS_ASSERT_EQUALS(SumFormula::fromString("Aa2A"), aa2a);
    QS_ASSERT_EQUALS(SumFormula::fromString("AaAAa"), aa2a);
    QS_ASSERT_EQUALS(SumFormula::fromString("AAaAa"), aa2a);
    QS_ASSERT_EQUALS(SumFormula::fromString("AAa2"), aa2a);

    auto a2 = SumFormula{{"A", 2}};
    QS_ASSERT_EQUALS(SumFormula::fromString("A2"), a2);
    QS_ASSERT_EQUALS(SumFormula::fromString("AA"), a2);

    auto a1b2c3 = SumFormula{{"A", 1}, {"B", 2}, {"C", 3}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AB2C3"), a1b2c3);

    auto a3minus = SumFormula{"A", 1, -3};
    QS_ASSERT_EQUALS(SumFormula::fromString("A-3"), a3minus);

    auto ab2_13minus = SumFormula{{"A", 1, -13}, {"B", 12}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AB12-13"), ab2_13minus);

    auto a2b5_10minus = SumFormula{{"A", 2, -10}, {"B", 5}};
    QS_ASSERT_EQUALS(SumFormula::fromString("A2-3B5-7"), a2b5_10minus);

    auto a2b5_15minus = SumFormula{{"A", 2, -15}, {"B", 5}};
    QS_ASSERT_EQUALS(SumFormula::fromString("A2-13B5-2"), a2b5_15minus);
  }

  void testValidityCheck() {
    bool valid = false;
    QString result;

    result = SumFormula::fromString("C3H5", &valid);
    auto c3h5 = SumFormula{{"C", 3}, {"H", 5}};
    TS_ASSERT(valid);
    QS_ASSERT_EQUALS(result, c3h5);

    result = SumFormula::fromString("C3", &valid);
    auto c3 = SumFormula{{"C", 3}};
    TS_ASSERT(valid);
    QS_ASSERT_EQUALS(result, c3);

    result = SumFormula::fromString("CaO", &valid);
    auto cao = SumFormula{{"Ca"}, {"O"}};
    TS_ASSERT(valid);
    QS_ASSERT_EQUALS(result, cao);

    result = SumFormula::fromString("Na2O", &valid);
    auto na2o = SumFormula{{"Na", 2}, {"O"}};
    TS_ASSERT(valid);
    QS_ASSERT_EQUALS(result, na2o);

    result = SumFormula::fromString("TosH", &valid);
    auto tosh = SumFormula{{"Tos"}, {"H"}};
    TS_ASSERT(valid);
    QS_ASSERT_EQUALS(result, tosh);

    result = SumFormula::fromString("", &valid);
    TS_ASSERT(!valid);
    QS_ASSERT_EQUALS(result, SumFormula());

    result = SumFormula::fromString("aC", &valid);
    TS_ASSERT(!valid);
    QS_ASSERT_EQUALS(result, SumFormula());

    result = SumFormula::fromString("C H 3", &valid);
    TS_ASSERT(!valid);
    QS_ASSERT_EQUALS(result, SumFormula());

    auto ab2_13minus = SumFormula{{"A", 1, -13}, {"B", 12}};
    QS_ASSERT_EQUALS(SumFormula::fromString("AB12-13", &valid), ab2_13minus);
    TS_ASSERT(valid);

    auto a2b5_10minus = SumFormula{{"A", 2, -10}, {"B", 5}};
    QS_ASSERT_EQUALS(SumFormula::fromString("A2-3B5-7", &valid), a2b5_10minus);
    TS_ASSERT(valid);
  }

  void testIsotopes() {
    auto formulaWithIsotopes = SumFormula{{"C", 1, 0, 13}, {"H", 4, 0, 2}};
    QS_ASSERT_EQUALS(formulaWithIsotopes.toHtml(), "<super>13</super>C<super>2</super>H<sub>4</sub>");
    QS_ASSERT_EQUALS(formulaWithIsotopes.toString(), "13C 2H4");

    auto formulaSortableIsotopes = SumFormula{{"C", 1, 0, 13}, {"C", 1, 0, 12}, {"C", 1, 0, 14}};
    QS_ASSERT_EQUALS(formulaSortableIsotopes.toHtml(), "<super>12</super>C<super>13</super>C<super>14</super>C");
    QS_ASSERT_EQUALS(formulaSortableIsotopes.toString(), "12C 13C 14C");
  }

  void testMassCalculation() {
    using std::isnan;

    auto abMasses = QMap<QString, qreal>{{"A", 3}, {"B", 10.5}};
    auto a2b = SumFormula{{"A", 2}, {"B"}};
    TS_ASSERT_EQUALS(a2b.calculateMass(abMasses), 16.5);

    auto a2c = SumFormula{{"A", 2}, {"C"}};
    TS_ASSERT_IS_NAN(a2c.calculateMass(abMasses));

    auto aIsotopeMasses = QMap<std::pair<QString,int>, qreal>{{{"A", 4}, 4.125}};
    auto a_4ab = SumFormula{{"A"}, {"A", 1, 0, 4}, {"B"}};
    auto a_4ab_mass = a_4ab.calculateMass(abMasses, aIsotopeMasses);
    TS_ASSERT_EQUALS(a_4ab_mass, 17.625);

    auto a_5ab_withInvalidIsotope = SumFormula{{"A"}, {"A", 1, 0, 5}, {"B"}};
    auto a_5ab_mass = a_5ab_withInvalidIsotope.calculateMass(abMasses, aIsotopeMasses);
    TS_ASSERT_IS_NAN(a_5ab_mass);
  }
};
