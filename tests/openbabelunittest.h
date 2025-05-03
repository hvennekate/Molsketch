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

static const char BABEL_LIBDIR_VARIABLE[] = "BABEL_LIBDIR"; // TODO take from OpenBabel directly

const char BUTANE_INCHI[] = "1S/C4H10/c1-3-4-2/h3-4H2,1-2H3";
const char BUTANE_FULL_INHCHI[] = "InChI=1S/C4H10/c1-3-4-2/h3-4H2,1-2H3\n";
const Core::Molecule &BUTANE{
  {{"C", 3}, {"C", 3}, {"C", 2}, {"C", 2}},
  {{0, 2}, {2, 3}, {3, 1}},
  ""
};

const char D_LACTIC_ACID_FULL_INCHI[] = "InChI=1S/C3H6O3/c1-2(4)3(5)6/h2,4H,1H3,(H,5,6)/t2-/m1/s1\n";
// TODO OpenBabel's gen2d actually generates a molecule here that is not in line with its own convention
// (tip of the hash bond is at the terminal methyl group; see perception.cpp:2440-2442
const Core::Molecule &D_LACTIC_ACID{
  {{"C", {2.6, 0.5}, 3}, {"C", {1.7, 0}, 1}, {"C", {0.9, 0.5}}, {"O", {1.7, -1}, 1}, {"O", {0.9, 1.5}}, {"O", 1}},
  {{1, 0, Core::Bond::Wedge}, {1, 2}, {1, 3}, {2, 4, Core::Bond::DoubleLegacy}, {2, 5}},
  "Lactic acid"
};

const char L_LACTIC_ACID_FULL_INCHI[] = "InChI=1S/C3H6O3/c1-2(4)3(5)6/h2,4H,1H3,(H,5,6)/t2-/m0/s1\n";
const Core::Molecule &L_LACTIC_ACID{
  {{"C", {2.6, 0.5}, 3}, {"C", {1.7, 0}, 1}, {"C", {0.9, 0.5}}, {"O", {1.7, -1}, 1}, {"O", {0.9, 1.5}}, {"O", 1}},
  {{1, 0, Core::Bond::Hash}, {1, 2}, {1, 3}, {2, 4, Core::Bond::DoubleLegacy}, {2, 5}},
  "Lactic acid"
};

const char CYANIDE_FULL_INCHI[] = "InChI=1S/CN/c1-2/q-1\n";
const Core::Molecule &CYANIDE{
  {{"C", {-.5, 0}, 0, -1}, {"N", {.5, 0}}},
  {{0, 1, Core::Bond::Triple}},
  "Cyanide anion"
};

class OpenBabelUnitTest : public CxxTest::TestSuite {
  void assertButane(const Core::Molecule &molecule) {
    assertListProperty(molecule.atoms(), &Core::Atom::element, {"C", "C", "C", "C"});
  }

  void assertLacticAcid(const Core::Molecule &molecule, Core::Bond::Type stereoBondType) {
    assertListProperty(molecule.atoms(), &Core::Atom::element, {"C", "C", "C", "O", "O", "O"});
    typedef std::tuple<unsigned, unsigned, unsigned> BondTuple;
    auto singleBond = Core::Bond::Single;
    assertListTransformed(molecule.bonds(), [](Core::Bond bond) { return std::make_tuple(bond.type(), bond.start(), bond.end()); },
      QList<BondTuple>{{stereoBondType, 0, 1}, {singleBond, 1, 2}, {singleBond, 1, 3}, {Core::Bond::DoubleLegacy, 2, 4}, {singleBond, 2, 5}});
  }
public:
#define MACRO_STRING_VALUE(X) STRING_VALUE(X) // TODO this should already be defined somewhere
#define STRING_VALUE(X) #X

  void setUp() override {
    qputenv (BABEL_LIBDIR_VARIABLE, MACRO_STRING_VALUE(MSK_OB_FORMATS_DIR));
  }

  void testConversionFromInChIString() { TS_SKIP("NEEDS FIXING!");
    auto molecule = fromInChI(BUTANE_INCHI);
    QSM_ASSERT("molecule not valid", molecule.isValid());
    assertButane(molecule);
  }

  void testReadingFile() { TS_SKIP("NEEDS FIXING!");
    std::istringstream input(BUTANE_FULL_INHCHI);
    auto molecule = loadFile(&input, "testinput.inchi");
    assertButane(molecule);
  }

  void testWritingFile() { TS_SKIP("NEEDS FIXING!");
    std::ostringstream output;
    bool result = saveFile(&output, "test.inchi", {BUTANE});
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(output.str(), BUTANE_FULL_INHCHI);
  } // TODO three dim

  void testReadingWedgeBond() { TS_SKIP("NEEDS FIXING!");
    std::istringstream input(L_LACTIC_ACID_FULL_INCHI);
    auto molecule = loadFile(&input, "testinput.inchi");
    assertLacticAcid(molecule, Core::Bond::Wedge);
  }

  void testWritingWedgeBond() { TS_SKIP("NEEDS FIXING!");
    std::ostringstream output;
    bool result = saveFile(&output, "test.inchi", {D_LACTIC_ACID});
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(output.str(), D_LACTIC_ACID_FULL_INCHI);
  }

  void testReadingHashBond() { TS_SKIP("NEEDS FIXING!");
    std::istringstream input(D_LACTIC_ACID_FULL_INCHI);
    auto molecule = loadFile(&input, "testinput.inchi");
    assertLacticAcid(molecule, Core::Bond::Hash);
  }

  void testWritingHashBond() { TS_SKIP("NEEDS FIXING!");
    std::ostringstream output;
    bool result = saveFile(&output, "test.inchi", {L_LACTIC_ACID});
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(output.str(), L_LACTIC_ACID_FULL_INCHI);
  }

  void testOptimizationOfCoordinates() { TS_SKIP("NEEDS FIXING!");
    auto optimizedCoords = optimizeCoordinates(BUTANE);
    TS_ASSERT_EQUALS(optimizedCoords.size(), 4);
    for (auto point : optimizedCoords) {
      TS_ASSERT_DIFFERS(std::abs(point.getX()), 0.);
      TS_ASSERT_DIFFERS(std::abs(point.getY()), 0.);
    }
  }

  void testOptimizationOfNullMolecule() { TS_SKIP("NEEDS FIXING!");
    auto optimizedCoords = optimizeCoordinates(Core::Molecule({}, {}));
    TS_ASSERT_EQUALS(optimizedCoords.size(), 0);
  }

  void testInputFormatsAvailable() { TS_SKIP("NEEDS FIXING!");
    QS_ASSERT_CONTAINS("GAMESS Input (*.gamin)", inputFormats());
    QS_ASSERT_CONTAINS("InChI format (*.inchi)", inputFormats());
  }

  void testOutputFormatsAvailable() { TS_SKIP("NEEDS FIXING!");
    QS_ASSERT_CONTAINS("MOPAC Internal (*.mopin)", outputFormats());
    QS_ASSERT_CONTAINS("InChI format (*.inchi)", outputFormats());
  }

  void testReadingImplicitHydrogens() { TS_SKIP("NEEDS FIXING!");
    std::istringstream input("InChI=1S/C2H5/c1-2/h1H2,2H3\n");
    auto molecule = loadFile(&input, "testinput.inchi");
    assertListProperty(molecule.bonds(), &Core::Bond::type, {Core::Bond::Single});
    assertListProperty(molecule.atoms(), &Core::Atom::charge, {0, 0});
    assertListProperty(molecule.atoms(), &Core::Atom::hAtoms, {2, 3});
  }

  void testWritingImplicitHydrogens() { TS_SKIP("NEEDS FIXING!");
    std::ostringstream output;
    bool result = saveFile(&output, "test.inchi", {Core::Molecule({{"C", 2}, {"C", 3}}, {{0, 1, Core::Bond::Single}})});
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(output.str(), "InChI=1S/C2H5/c1-2/h1H2,2H3\n");
  }

  void testReadingCharge() { TS_SKIP("NEEDS FIXING!");
    std::istringstream input(CYANIDE_FULL_INCHI);
    auto molecule = loadFile(&input, "testinput.inchi");
    assertListProperty(molecule.atoms(), &Core::Atom::element, {"C", "N"});
    assertListProperty(molecule.atoms(), &Core::Atom::charge, {-1, 0});
    assertListProperty(molecule.bonds(), &Core::Bond::type, {Core::Bond::Triple});
    assertListProperty(molecule.bonds(), &Core::Bond::start, {0});
    assertListProperty(molecule.bonds(), &Core::Bond::end, {1});
  }

  void testWritingCharge() { TS_SKIP("NEEDS FIXING!");
    std::ostringstream output;
    bool result = saveFile(&output, "test.inchi", {CYANIDE});
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(output.str(), CYANIDE_FULL_INCHI);
  }

  void testReadMultipleMolecules() { TS_SKIP("NEEDS FIXING!");
    std::istringstream input("InChI=1S/C4H10.CN/c1-3-4-2;1-2/h3-4H2,1-2H3;/q;-1\n");
    auto molecule = loadFile(&input, "testinput.inchi");

    // it is acceptable here to have one "big" united molecule,
    // as the disjoint parts will be split in MainWindow::readSceneUsingOpenBabel
    // (should possibly be moved
    assertListProperty(molecule.atoms(), &Core::Atom::element, {"C", "C", "C", "C", "C", "N"});
    assertListProperty(molecule.bonds(), &Core::Bond::start, {0, 1, 2, 4});
    assertListProperty(molecule.bonds(), &Core::Bond::end, {2, 3, 3, 5});
  }

  void testWriteMultipleMolecules() { TS_SKIP("NEEDS FIXING!");
    std::ostringstream output;
    bool result = saveFile(&output, "test.inchi", {BUTANE, CYANIDE});
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(output.str(), "InChI=1S/C4H10.CN/c1-3-4-2;1-2/h3-4H2,1-2H3;/q;-1\n");
  }
};
