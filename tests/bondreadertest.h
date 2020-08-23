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
#include <molecule.h>
#include <atom.h>
#include <bond.h>
#include <QSet>

using namespace Molsketch;

class BondReaderTest : public CxxTest::TestSuite {
  Atom *atom1, *atom2;
  Bond *bond;
  Molecule *molecule;

public:
  void setUp() {
    atom1 = new Atom();
    atom2 = new Atom();
    bond = new Bond();
    molecule = new Molecule();
    molecule->addAtom(atom1);
    molecule->addAtom(atom2);
    molecule->addBond(bond);
  }

  void tearDown() {
    delete molecule;
  }

  void testReadLegacyHashBond() {
    QXmlStreamReader in("<bond atomRefs2=\"a1 a2\" order=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\">"
                        "<bondStereo>H</bondStereo>"
                        "</bond>");
    in.readNextStartElement();
    bond->readXml(in);
    TS_ASSERT_EQUALS(bond->beginAtom(), atom1);
    TS_ASSERT_EQUALS(bond->endAtom(), atom2);
    TS_ASSERT_EQUALS(bond->bondType(), Bond::Hash);
  }

  void testReadLegacyWedgeBond() {
    QXmlStreamReader in("<bond atomRefs2=\"a1 a2\" order=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\">"
                        "<bondStereo>W</bondStereo>"
                        "</bond>");
    in.readNextStartElement();
    bond->readXml(in);
    TS_ASSERT_EQUALS(bond->beginAtom(), atom1);
    TS_ASSERT_EQUALS(bond->endAtom(), atom2);
    TS_ASSERT_EQUALS(bond->bondType(), Bond::Wedge);
  }
};
