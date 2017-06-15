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

  void testCorrectIconNameAndXmlAreProduced() {
    moleculeToProduce->setName("Testname");
    // TODO icon test doesn't seem to work
    QS_ASSERT_EQUALS(QByteArray((char*) item->icon().pixmap(64).toImage().bits()),
                     QByteArray((char*) renderMolecule(*moleculeToProduce).toImage().bits()));
    QS_ASSERT_EQUALS(item->name(), "Testname");

    QByteArray writtenXml;
    QXmlStreamWriter xmlWriter(&writtenXml);
    item->writeXml(xmlWriter);
    QS_ASSERT_EQUALS(writtenXml, "<molecule name=\"Testname\"><atomArray/><bondArray/></molecule>")
  }

  void testMultipleCallsToIconNameOrXmlLeadToOnlyASingleProduceMoleculeCall() {
    item->icon();
    item->icon();
    item->name();
    item->name();
    QByteArray xml;
    QXmlStreamWriter writer(&xml);
    item->writeXml(writer);
    item->writeXml(writer);

    TS_ASSERT_EQUALS(produceMoleculeInvocationCounter, 1);
  }

  void testNullMoleculeReturnIsHandledGracefully() {
    delete moleculeToProduce;
    moleculeToProduce = nullptr;
    item->icon();
    item->name();
    QByteArray xml;
    QXmlStreamWriter writer(&xml);
    item->writeXml(writer);
  }

  void testIconNameAndXmlAreReturnedFromXml() {
    // TODO
  }

  void testIconNameAndXmlAreReturnedFromInChI() {
    // TODO mock obabeliface
  }
};
