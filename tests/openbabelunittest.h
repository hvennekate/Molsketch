/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate                               *
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
#include <obabeliface.h>
#include <molecule.h>
#include "utilities.h"
using namespace Molsketch;
#include <QDebug>

class OpenBabelUnitTest : public CxxTest::TestSuite {
public:
  void testConversionFromSmilesString() {
    qputenv("BABEL_LIBDIR", "/usr/lib64/openbabel/2.4.1"); // TODO make configurable
    Molecule* molecule = assertNotNull(fromInChI("1S/C4H10/c1-3-4-2/h3-4H2,1-2H3"));
    TS_ASSERT_EQUALS(molecule->atoms().size(), 4);
    for(Atom* atom : molecule->atoms())
      TS_ASSERT_EQUALS(atom->element(), "C");

    delete molecule;
  }
};
