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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/TestSuite.h>
#include <molecule.h>
#include <atom.h>
#include <bond.h>
#include <electronsystem.h>
#include "utilities.h"

using namespace Molsketch;

CLASS_FOR_TESTING(PiElectrons)

class MoleculeUnitTest : public CxxTest::TestSuite {
  Molecule *molecule;
  Atom *atomA, *atomB, *atomC, *atomD, *atomE, *atomF;

  void setUpAtoms() {
    atomA = new Atom;
    atomB = new Atom;
    atomC = new Atom;
    atomD = new Atom;
    atomE = new Atom;
    atomF = new Atom;
    molecule->addAtom(atomA);
    molecule->addAtom(atomB);
    molecule->addAtom(atomC);
    molecule->addAtom(atomD);
    molecule->addAtom(atomE);
    molecule->addAtom(atomF);
  }

  void performAssertionsOnPiElectronSystem(const ElectronSystem* electronSystem) {
    performAssertionsOnPiElectronSystem(electronSystem, atomA, atomB);
  }

  void performAssertionsOnPiElectronSystem(const ElectronSystem* electronSystem, Atom* first, Atom* second) {
    performAssertionsOnPiElectronSystem(electronSystem, {first, second});
  }

  void performAssertionsOnPiElectronSystem(const ElectronSystem* electronSystem, QSet<Atom*> expectedAtoms, int additionalElectrons = 0) {
    QS_ASSERT_EQUALS(toSet(electronSystem->atoms()), expectedAtoms);
    TS_ASSERT_EQUALS(electronSystem->numAtoms(), expectedAtoms.size());
    TS_ASSERT_EQUALS(electronSystem->numElectrons(), expectedAtoms.size() + additionalElectrons);
    TS_ASSERT_EQUALS(electronSystem->type(), ElectronSystem::PiType);
  }

  void performAssertionsOnAtomicElectronSystem(const ElectronSystem* electronSystem, int electonCount) {
    TS_ASSERT_EQUALS(electronSystem->numAtoms(), 1);
    TS_ASSERT_EQUALS(electronSystem->atoms().first(), atomA);
    TS_ASSERT_EQUALS(electronSystem->numElectrons(), electonCount);
    TS_ASSERT_EQUALS(electronSystem->type(), ElectronSystem::PiType);
  }

public: // TODO serialization tests
  void setUp() {
    molecule = new Molecule;
  }

  void tearDown() {
    delete molecule;
  }

  void testElectronSystemsAreClearedWhenUpdating() {
    molecule->m_electronSystemsUpdate = true;
    molecule->m_electronSystems << new PiElectronsForTesting;
    molecule->updateElectronSystems();
    TS_ASSERT(molecule->m_electronSystems.isEmpty());
    TS_ASSERT_EQUALS(PiElectronsForTesting::instanceCounter, 0);
  }

  void testNoPiElectronSystemForSingleBond() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::Single);
    molecule->updateElectronSystems();
    TS_ASSERT(molecule->m_electronSystems.empty());
  }

  void testOnePiElectronSystemForDoubleBond() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->updateElectronSystems();
    // TODO array/container/iterable/iterator assertion
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first());
  }

  void testTwoPiElectronSystemsForTripleBond() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::Triple);
    molecule->updateElectronSystems();
    // TODO array/container/iterable/iterator assertion
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first());
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last());
  }

  void testTwoPiElectronSystemsForAlleneBonds() {
    setUpAtoms();
    molecule->addBond(atomB, atomA, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::DoubleSymmetric);
    molecule->updateElectronSystems();
    // TODO array/container/iterable/iterator assertion
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first());
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), {atomB, atomC});
  }

  void testTwoPiElectronSystemsForAlleneBondsReverse() {
    setUpAtoms();
    molecule->addBond(atomB, atomA, Bond::DoubleSymmetric);
    molecule->addBond(atomC, atomB, Bond::DoubleSymmetric);
    molecule->updateElectronSystems();
    // TODO array/container/iterable/iterator assertion
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first());
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), {atomB, atomC});
  }

  void testPiElectronSystemForAtomWithSingleUnboundElectron() {
    setUpAtoms();
    atomA->setNumUnpairedElectrons(1);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.first(), 1);
  }

  void testPiElectronSystemForAtomWithTwoUnboundElectrons() {
    setUpAtoms();
    atomA->setNumUnpairedElectrons(2);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.first(), 2);
  }

  void testPiElectronSystemForAtomWithThreeUnboundElectrons() {
    setUpAtoms();
    atomA->setNumUnpairedElectrons(3);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.first(), 2);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.last(), 1);
  }

  void testPiElectronSystemForAtomWithFourUnboundElectrons() {
    setUpAtoms();
    atomA->setNumUnpairedElectrons(4);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.first(), 2);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.last(), 2);
  }

  void testPiElectronSystemsNotMergedIfShareAtom() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomC, atomB, Bond::DoubleSymmetric);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first());
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), atomC, atomB);
  }

  void testPiElectronSystemsMergedIfNeighboringAtoms() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::Single);
    molecule->addBond(atomC, atomD, Bond::DoubleSymmetric);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC, atomD});
  }

  void testPiElectronSystemsNotMergedIfAtomPartOfPiBond() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    atomA->setNumUnpairedElectrons(1);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first());
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.last(), 1);
  }

  void testPiElectronSystemMergedForAtomAndNeighboringBond() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::Single);
    atomC->setNumUnpairedElectrons(1);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC});
  }

  void testThreePiElectronsOnAtomOnlyTwoGetMerged() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::Single);
    molecule->addBond(atomB, atomC, Bond::DoubleSymmetric);
    atomA->setNumUnpairedElectrons(3);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC}, 1);
    performAssertionsOnAtomicElectronSystem(molecule->m_electronSystems.last(), 1);
  }

  void testBenzene() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::Single);
    molecule->addBond(atomC, atomD, Bond::DoubleSymmetric);
    molecule->addBond(atomD, atomE, Bond::Single);
    molecule->addBond(atomE, atomF, Bond::DoubleSymmetric);
    molecule->addBond(atomF, atomA, Bond::Single);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC, atomD, atomE, atomF});
  }

  void testBenzyneA() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::Single);
    molecule->addBond(atomC, atomD, Bond::Triple);
    molecule->addBond(atomD, atomE, Bond::Single);
    molecule->addBond(atomE, atomF, Bond::DoubleSymmetric);
    molecule->addBond(atomF, atomA, Bond::Single);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC, atomD, atomE, atomF});
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), {atomC, atomD});
  }

  void testBenzyneB() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::DoubleSymmetric);
    molecule->addBond(atomC, atomD, Bond::DoubleSymmetric);
    molecule->addBond(atomD, atomE, Bond::Single);
    molecule->addBond(atomE, atomF, Bond::DoubleSymmetric);
    molecule->addBond(atomF, atomA, Bond::Single);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC, atomD, atomE, atomF});
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), {atomB, atomC});
  }

  void testPyridine() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::Single);
    molecule->addBond(atomC, atomD, Bond::DoubleSymmetric);
    molecule->addBond(atomD, atomE, Bond::Single);
    molecule->addBond(atomE, atomF, Bond::DoubleSymmetric);
    molecule->addBond(atomF, atomA, Bond::Single);
    atomA->setNumUnpairedElectrons(2);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 2);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), {atomA}, 1);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.first(), {atomA, atomB, atomC, atomD, atomE, atomF});
  }

  void testPyrrole() {
    setUpAtoms();
    molecule->addBond(atomA, atomB, Bond::DoubleSymmetric);
    molecule->addBond(atomB, atomC, Bond::Single);
    molecule->addBond(atomC, atomD, Bond::Single);
    molecule->addBond(atomD, atomE, Bond::DoubleSymmetric);
    molecule->addBond(atomE, atomA, Bond::Single);
    atomC->setNumUnpairedElectrons(2);
    molecule->updateElectronSystems();
    TS_ASSERT_EQUALS(molecule->m_electronSystems.size(), 1);
    performAssertionsOnPiElectronSystem(molecule->m_electronSystems.last(), {atomA, atomB, atomC, atomD, atomE}, 1);
  }

  void testSumFormula() {
    setUpAtoms();
    QS_ASSERT_EQUALS(molecule->sumFormula(), SumFormula());

    auto atomA = new Atom(QPointF(), "CH2"), atomB = new Atom(QPointF(), "CH3");
    Molecule c2h5({atomA, atomB}, {});
    SumFormula c2h5sum({{"C", 2}, {"H", 5}});
    QS_ASSERT_EQUALS(c2h5.sumFormula(), c2h5sum);
  }

  void testInitialTooltipSetting() {
    auto atomA = new Atom(QPointF(), "CH2"), atomB = new Atom(QPointF(), "CH3");
    Molecule c2h5({atomA, atomB}, {});
    QS_ASSERT_EQUALS(c2h5.toolTip(), "C<sub>2</sub>H<sub>5</sub>");
  }

  void testToolTipSettingAfterAddingAtom() {
    auto atomA = new Atom(QPointF(), "CH2"), atomB = new Atom(QPointF(), "CH3");
    Molecule c2h5(QSet<Atom*>{atomA}, {});
    QS_ASSERT_EQUALS(c2h5.toolTip(), "CH<sub>2</sub>");
    c2h5.addAtom(atomB);
    QS_ASSERT_EQUALS(c2h5.toolTip(), "C<sub>2</sub>H<sub>5</sub>");
  }

  void testToolTipSettingAfterDeletingAtom() {
    auto atomA = new Atom(QPointF(), "CH2"), atomB = new Atom(QPointF(), "CH3");
    Molecule c2h5({atomA, atomB}, {});
    QS_ASSERT_EQUALS(c2h5.toolTip(), "C<sub>2</sub>H<sub>5</sub>");
    c2h5.delAtom(atomB);
    delete atomB;
    QS_ASSERT_EQUALS(c2h5.toolTip(), "CH<sub>2</sub>");
  }

  void testToolTipSettingAfterChangingAtomSymbol() {
    auto atomA = new Atom(QPointF(), "CH2");
    Molecule c2h5(QSet<Atom*>{atomA}, {});
    atomA->setElement("NH3");
    QS_ASSERT_EQUALS(c2h5.toolTip(), "H<sub>3</sub>N"); // TODO change ordering in hydrogen compounds
  }

  void testToolTipSettingAfterChangingHydrogenCountOfAtom() {
    auto atomA = new Atom(QPointF(), "C");
    Molecule c2h5(QSet<Atom*>{atomA}, {});
    atomA->setNumImplicitHydrogens(3);
    QS_ASSERT_EQUALS(c2h5.toolTip(), "CH<sub>3</sub><super>+</super>");
  }

  void testToolTipSettingAfterChangingChargeOfAtom() {
    auto atomA = new Atom(QPointF(), "C");
    Molecule c2h5(QSet<Atom*>{atomA}, {});
    atomA->setCharge(-3);
    QS_ASSERT_EQUALS(c2h5.toolTip(), "CH<sub>4</sub><super>3-</super>");
  }

  void testAddingBondsUpdatesToolTip() {
    auto atomA = new Atom(QPointF(), "C"),
        atomB =  new Atom(QPointF(), "C");
    Molecule molecule({atomA, atomB}, {});
    molecule.addBond(atomA, atomB);
    QS_ASSERT_EQUALS(molecule.toolTip(), "C<sub>2</sub>H<sub>6</sub>");
    QS_ASSERT_EQUALS(molecule.sumFormula().toHtml(), "C<sub>2</sub>H<sub>6</sub>");
  }

  void testDeletingBondsUpdatesToolTip() {
    auto atomA = new Atom(QPointF(), "C"),
        atomB =  new Atom(QPointF(), "C");
    auto bond = new Bond(atomA, atomB);
    Molecule molecule({atomA, atomB}, {bond});
    molecule.delBond(bond);
    QS_ASSERT_EQUALS(molecule.toolTip(), "C<sub>2</sub>H<sub>8</sub>");
    QS_ASSERT_EQUALS(molecule.sumFormula().toHtml(), "C<sub>2</sub>H<sub>8</sub>");
  }

  void testAtomsAreIndexedAfterAdding() {
    Molecule molecule;
    auto atom = new Atom();
    atom->setParentItem(&molecule);
    QS_ASSERT_EQUALS(atom->index(), "a1");
  }

  void testAtomsAreIndexedAfterRemoval() {
    Molecule molecule;
    auto atomA = new Atom(), atomB = new Atom();
    atomA->setParentItem(&molecule);
    atomB->setParentItem(&molecule);
    auto initialIndexOfAtomB = atomB->index();
    QS_ASSERT_EQUALS(initialIndexOfAtomB, "a2");
    atomA->setParentItem(nullptr);
    delete atomA;
    QS_ASSERT_EQUALS(atomB->index(), "a1");
    QS_ASSERT_NOT_EQUALS(atomB->index(), initialIndexOfAtomB);
  }
};
