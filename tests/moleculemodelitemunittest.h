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
#include <mocks.h>
#include <molecule.h>
#include <utilities.h>
#include <moleculemodelitem.h>

using namespace Molsketch;

class MoleculeModelItemUnitTest : public CxxTest::TestSuite {
  CLASS_FOR_TESTING_WITH_FUNCTIONS(MoleculeModelItem,
    public:
      MoleculeModelItemForTesting(){}
      MOCK_CONST(Molecule*, produceMolecule, ,))

  int produceMoleculeInvocationCounter;
  Molecule* moleculeToProduce;
  MoleculeModelItemForTesting* item;
public:
  void setUp() {
    item = new MoleculeModelItemForTesting;
    produceMoleculeInvocationCounter = 0;
    item->produceMoleculeCallback =
        [&] { ++produceMoleculeInvocationCounter; return moleculeToProduce; };
    moleculeToProduce = new Molecule();
  }

  void tearDown() {
    if (!produceMoleculeInvocationCounter) delete moleculeToProduce;
    delete item;
  }

  void testCorrectIconNameAndMoleculeAreProduced() {
    moleculeToProduce->setName("Testname");
    TS_ASSERT(!item->icon().isNull());
    QS_ASSERT_EQUALS(item->name(), "Testname");

    QByteArray writtenXml;
    QXmlStreamWriter xmlWriter(&writtenXml);
    const Molecule* molecule = item->getMolecule();
    QS_ASSERT_ON_POINTER(const Molecule, molecule, getName(), "Testname");
  }

  void testMultipleCallsToIconNameOrGetMoleculeLeadToOnlyASingleProduceMoleculeCall() {
    item->icon();
    item->icon();
    item->name();
    item->name();
    QByteArray xml;
    QXmlStreamWriter writer(&xml);
    item->getMolecule();
    item->getMolecule();

    TS_ASSERT_EQUALS(produceMoleculeInvocationCounter, 1);
  }

  void testNullMoleculeReturnIsHandledGracefully() {
    delete moleculeToProduce;
    moleculeToProduce = nullptr;
    item->icon();
    item->name();
    QByteArray xml;
    QXmlStreamWriter writer(&xml);
    item->getMolecule();
  }

  void testIconNameAndXmlAreReturnedFromXml() {
    // TODO
  }

  void testIconNameAndXmlAreReturnedFromInChI() {
    // TODO mock obabeliface
  }
};
