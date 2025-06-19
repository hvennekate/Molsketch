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

#ifndef COREMOLECULETEST_H
#define COREMOLECULETEST_H

#include "coremolecule.h"
#include "coreatom.h"
#include "corebond.h"
#include <cxxtest/TestSuite.h>
#include "utilities.h"

class CoreMoleculeTest : public CxxTest::TestSuite {
public:

  void testCreation() {
    using Molsketch::Core::Atom;
    using Molsketch::Core::Bond;
    auto bond = Bond(0, 1, Bond::DativeDot);
    Molsketch::Core::Molecule molecule({Atom("C"), Atom("N", {1.5, 3})},
                                       {bond},
                                       "test molecule");

    TS_ASSERT_EQUALS(molecule.atoms().size(), 2);
    QS_ASSERT_EQUALS(molecule.atoms().at(0).element(), "C");
    QS_ASSERT_EQUALS(molecule.atoms().at(1).element(), "N");
    TS_ASSERT_EQUALS(molecule.bonds().size(), 1);
    TS_ASSERT_EQUALS(molecule.bonds().at(0).start(), 0);
    TS_ASSERT_EQUALS(molecule.bonds().at(0).end(), 1);
    QS_ASSERT_EQUALS(molecule.name(), "test molecule")

    auto otherMolecule(molecule);
    TS_ASSERT_EQUALS(otherMolecule.atoms().size(), 2);
    QS_ASSERT_EQUALS(otherMolecule.atoms().at(0).element(), "C");
    QS_ASSERT_EQUALS(otherMolecule.atoms().at(1).element(), "N");
    TS_ASSERT_EQUALS(otherMolecule.bonds().size(), 1);
    TS_ASSERT_EQUALS(otherMolecule.bonds().at(0).start(), 0);
    TS_ASSERT_EQUALS(otherMolecule.bonds().at(0).end(), 1);
    QS_ASSERT_EQUALS(otherMolecule.name(), "test molecule")
  }

  void testHandlingInQVector() {
    using Molsketch::Core::Atom;
    using Molsketch::Core::Bond;
    auto bond = Bond(0, 1, Bond::DativeDot);
    Molsketch::Core::Molecule molecule({Atom("C"), Atom("N", {1.5, 3})},
                                       {bond},
                                       "test molecule");

    QVector<Molsketch::Core::Molecule> molecules;
    molecules << molecule;
    auto moleculeFromVector = molecules.last();
    TS_ASSERT_EQUALS(moleculeFromVector.atoms().size(), 2);
    QS_ASSERT_EQUALS(moleculeFromVector.atoms().at(0).element(), "C");
    TS_ASSERT_EQUALS(moleculeFromVector.atoms().at(0).position().getX(), 0);
    TS_ASSERT_EQUALS(moleculeFromVector.atoms().at(0).position().getY(), 0);
    TS_ASSERT_EQUALS(moleculeFromVector.atoms().at(1).position().getX(), 1.5);
    TS_ASSERT_EQUALS(moleculeFromVector.atoms().at(1).position().getY(), 3);
    QS_ASSERT_EQUALS(moleculeFromVector.atoms().at(1).element(), "N");
    TS_ASSERT_EQUALS(moleculeFromVector.bonds().size(), 1);
    TS_ASSERT_EQUALS(moleculeFromVector.bonds().at(0).start(), 0);
    TS_ASSERT_EQUALS(moleculeFromVector.bonds().at(0).end(), 1);
    QS_ASSERT_EQUALS(moleculeFromVector.name(), "test molecule")
  }

  void testAveragePosition() {
    using Molsketch::Core::Atom;
    using Molsketch::Core::Bond;
    auto bond = Bond(0, 1, Bond::DativeDot);
    Molsketch::Core::Molecule molecule({Atom("C"), Atom("N", {1.5, 3})},
                                       {bond},
                                       "test molecule");
    Molsketch::Core::Position expectedAveragePosition(0.75, 1.5);
    QS_ASSERT_EQUALS(molecule.center(), expectedAveragePosition);
  }

  void testAveragePositionForInvalidMolecule() {
    Molsketch::Core::Molecule molecule({}, {});

    QS_ASSERT_EQUALS(molecule.center(), Molsketch::Core::Position());
  }

  void testShift() {
    using Molsketch::Core::Atom;
    using Molsketch::Core::Bond;
    auto bond = Bond(0, 1, Bond::DativeDot);
    Molsketch::Core::Molecule molecule({Atom("C"), Atom("N", {1.5, 3})},
                                       {bond},
                                       "test molecule");

    auto shiftedMolecule = molecule.shiftedBy({-3.5, +10.5});
    TS_ASSERT_EQUALS(shiftedMolecule.atoms().size(), 2);
    TS_ASSERT_EQUALS(shiftedMolecule.atoms().at(0).position().getX(), -3.5);
    TS_ASSERT_EQUALS(shiftedMolecule.atoms().at(0).position().getY(), 10.5);
    TS_ASSERT_EQUALS(shiftedMolecule.atoms().at(1).position().getX(), -2);
    TS_ASSERT_EQUALS(shiftedMolecule.atoms().at(1).position().getY(), 13.5);
  }
};

#endif
