/***************************************************************************
 *   Copyright (C) 2021 by Hendrik Vennekate                               *
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
#include <molecule.h>
#include <molscene.h>

#include "listassertion.h"

using namespace Molsketch;

const QString &SCENE_WITH_LONE_ATOM(
    "<molscene>"
    "<atom"
    " id=\"a1\""
    " elementType=\"A\""
    "/>"
    "</molscene>");

const QString &SCENE_WITH_BOND_WITH_MISSING_INDEX(
    "<molscene>"
    " <molecule>"
    "  <atomArray>"
    "   <atom id=\"a1\" elementType=\"A\"/>"
    "   <atom id=\"a2\" elementType=\"B\"/>"
    "  </atomArray>"
    "  <bondArray>"
    "   <bond atomRefs=\"a1 \"/>"
    "  </bondArray>"
    " </molecule>"
    "</molscene>");

const QString &SCENE_WITH_BOND_WITHOUT_INDEXES(
    "<molscene>"
    " <molecule>"
    "  <atomArray>"
    "   <atom id=\"a1\" elementType=\"A\"/>"
    "   <atom id=\"a2\" elementType=\"B\"/>"
    "  </atomArray>"
    "  <bondArray>"
    "   <bond atomRefs=\" \"/>"
    "  </bondArray>"
    " </molecule>"
    "</molscene>");

const QString &SCENE_WITH_BOND_WITH_INVALID_INDEX(
    "<molscene>"
    " <molecule>"
    "  <atomArray>"
    "   <atom id=\"a1\" elementType=\"A\"/>"
    "   <atom id=\"a2\" elementType=\"B\"/>"
    "  </atomArray>"
    "  <bondArray>"
    "   <bond atomRefs=\"a1 b3\"/>"
    "  </bondArray>"
    " </molecule>"
    "</molscene>");

class FileInputOutputTest : public CxxTest::TestSuite {
  MolScene *scene;

  void readScene(const QString &input) {
    QXmlStreamReader xml(input);
    xml.readNext();
    xml >> *scene;
  }
public:
  void setUp() override {
    scene = new MolScene;
  }

  void tearDown() override {
    delete scene;
  }

  void testReadingSceneWithAtomWithoutMolecule_wrapsMoleculeAroundThatAtom() {
    readScene(SCENE_WITH_LONE_ATOM);
    TS_ASSERT_EQUALS(scene->items().size(), 2);
    QS_ASSERT_ON_POINTER(Atom, scene->items().first(), molecule(), dynamic_cast<Molecule*>(scene->items().last()));
  }

  void testReadingBondWithMissingIndex_bondGetsDropped() {
    readScene(SCENE_WITH_BOND_WITH_MISSING_INDEX);
    assertThat(scene->items()).selectingType<Bond*>().isEmpty();
    assertThat(scene->items()).selectingType<Molecule*>().hasSize(2);
  }

  void testReadingBondWithoutIndexes_bondGetsDropped() {
    readScene(SCENE_WITH_BOND_WITHOUT_INDEXES);
    assertThat(scene->items()).selectingType<Bond*>().isEmpty();
    assertThat(scene->items()).selectingType<Molecule*>().hasSize(2);
  }

  void testReadingBondWithInvalidIndex_bondGetsDropped() {
    readScene(SCENE_WITH_BOND_WITH_INVALID_INDEX);
    assertThat(scene->items()).selectingType<Bond*>().isEmpty();
    assertThat(scene->items()).selectingType<Molecule*>().hasSize(2);
  }
};
