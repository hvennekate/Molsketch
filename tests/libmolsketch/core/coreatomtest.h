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

#ifndef COREATOMTEST_H
#define COREATOMTEST_H

#include <coreatom.h>
#include <cxxtest/TestSuite.h>
#include "utilities.h"

class CoreAtomTest : public CxxTest::TestSuite {
public:

  void testCreation() {
    Molsketch::Core::Atom atom("Ba", {5,3.5}, 3, -4);
    QS_ASSERT_EQUALS(atom.element(), "Ba");
    Molsketch::Core::Position x{5, 3.5};
    TS_ASSERT_EQUALS(atom.position(), x);
    TS_ASSERT_EQUALS(atom.hAtoms(), 3);
    TS_ASSERT_EQUALS(atom.charge(), -4);

    auto otherAtom(atom);
    QS_ASSERT_EQUALS(otherAtom.element(), "Ba");
    TS_ASSERT_EQUALS(otherAtom.position(), x);
    TS_ASSERT_EQUALS(otherAtom.hAtoms(), 3);
    TS_ASSERT_EQUALS(otherAtom.charge(), -4);
  }
};

#endif
