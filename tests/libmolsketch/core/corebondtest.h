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

#ifndef COREBONDTEST_H
#define COREBONDTEST_H

#include <core/corebond.h>
#include <cxxtest/TestSuite.h>

class CoreBondTest : public CxxTest::TestSuite {
public:

  void testCreation() {
    Molsketch::Core::Bond bond(5, 3, Molsketch::Core::Bond::DativeDot);

    TS_ASSERT_EQUALS(5, bond.start());
    TS_ASSERT_EQUALS(3, bond.end());
    TS_ASSERT_EQUALS(Molsketch::Core::Bond::DativeDot, bond.type());

    auto otherBond(bond);
    TS_ASSERT_EQUALS(5, otherBond.start());
    TS_ASSERT_EQUALS(3, otherBond.end());
    TS_ASSERT_EQUALS(Molsketch::Core::Bond::DativeDot, otherBond.type());
  }
};

#endif
