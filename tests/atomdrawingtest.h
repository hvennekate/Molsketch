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
#include <atom.h>
#include <molscene.h>
#include <molecule.h>
#include <QSet>
#include "xmlassertion.h"
using namespace Molsketch;
using XmlAssert::assertThat;

const char* XENON = "Xe";

class AtomDrawingTest : public CxxTest::TestSuite {
  MolScene *scene;
  Molecule *molecule;
  Atom *atom;
public:
  void setUp() {
    atom = new Atom(QPointF(), XENON);
    molecule = new Molecule(QSet<Atom*>{atom}, QSet<Bond*>());
    scene = new MolScene;
    scene->addItem(molecule);
  }

  void tearDown() {
    delete scene;
  }

  void testRegularAtomIsDrawn() {
    assertThat(scene->toSvg())->hasNodes("svg/g/g/text")->times(1)->haveTexts()->exactly({"Xe"});
  }

  void testNewmanAtomSymbolIsNotDrawn() { // TODO define precedence with regard to colored circle/square/wireframe
    // TODO check color
    atom->setNewmanDiameter(6);
    QString svg = scene->toSvg();
    assertThat(svg)->hasNodes("svg/g/g/text")->none();
    assertThat(svg)->hasNodes("svg/g/g/rect")->none();
    auto circleAssert = assertThat(svg)->hasNodes("svg/g/g/circle")->exactlyOne();
    circleAssert->haveAttribute("r")->exactly({"3"});
    circleAssert->haveAttribute("cx")->exactly({"0"});
    circleAssert->haveAttribute("cy")->exactly({"0"});
    // TODO why does cx and cy get moved to 0.5 if diameter is odd?
  }

  void testDrawingColoredCircle() {
    scene->setRenderMode(MolScene::RenderColoredCircles);
    QString svg = scene->toSvg();
    assertThat(svg)->hasNodes("svg/g/g/text")->none();
    assertThat(svg)->hasNodes("svg/g/g/rect")->none();
    auto  circleAssert = assertThat(svg)->hasNodes("svg/g/g/circle")->exactlyOne();
    circleAssert->haveAttribute("r")->exactly({"10"});
    circleAssert->haveAttribute("cx")->exactly({"0"});
    circleAssert->haveAttribute("cy")->exactly({"0"});

  }

  void testDrawingColoredSquare() {
    scene->setRenderMode(MolScene::RenderColoredSquares);
    QString svg = scene->toSvg();
    assertThat(svg)->hasNodes("svg/g/g/text")->none();
    assertThat(svg)->hasNodes("svg/g/g/circle")->none();
    auto  rectAssert = assertThat(svg)->hasNodes("svg/g/g/rect")->exactlyOne();
    rectAssert->haveAttribute("x")->exactly({"-10"});
    rectAssert->haveAttribute("y")->exactly({"-10"});
    rectAssert->haveAttribute("width")->exactly({"20"});
    rectAssert->haveAttribute("height")->exactly({"20"});
  }

  void testDrawingColoredWireframe() {
    scene->setRenderMode(MolScene::RenderColoredWireframe);
    QString svg = scene->toSvg();
    assertThat(svg)->hasNodes("svg/g/g/text")->none();
    assertThat(svg)->hasNodes("svg/g/g/rect")->none();
    assertThat(svg)->hasNodes("svg/g/g/circle")->none();
  }

  void testCenteringOfNewmanAtom() {
    atom->setNewmanDiameter(1);
    assertCircleIsCentered();
    atom->setNewmanDiameter(2);
    assertCircleIsCentered();
    atom->setNewmanDiameter(3);
    assertCircleIsCentered();
  }

  void assertCircleIsCentered() {
    auto svg = scene->toSvg();
    auto circleAssert = assertThat(svg)->hasNodes("svg/g/g/circle")->exactlyOne();
    circleAssert->haveAttribute("cx")->exactly({"0"});
    circleAssert->haveAttribute("cy")->exactly({"0"});
  }

  // TODO check unbound electrons and charge are drawn in the same color
  // TODO check rendering of Hydrogen atoms (including left or right of element)
  // TODO check that rendering mode takes precedence over Newman
};
