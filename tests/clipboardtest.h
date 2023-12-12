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
#include <QApplication>
#include <QClipboard>
#include <molscene.h>
#include <QMimeData>
#include <QSet>
#include <molecule.h>
#include <frame.h>
#include "utilities.h"
#include "xmlassertion.h"

using namespace Molsketch;

const QString MIME_TYPE("molecule/molsketch");
const QString MIME_ATOM(
    "<atom"
    " id=\"a1\""
    " elementType=\"Ca\""
    " userCharge=\"0\""
    " disableHydrogens=\"0\""
    " hydrogens=\"0\""
    " shapeType=\"0\""
    " hydrogenAlignment=\"0\""
    " colorR=\"0\""
    " colorG=\"0\""
    " colorB=\"0\""
    " scalingParameter=\"1\""
    " zLevel=\"3\""
    " coordinates=\"5,3\"/>"
    );
const QString CLIPBOARD_CONTENT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<molecule name=\"\">"
                                  "<atomArray>"
                                  + MIME_ATOM +
                                  "</atomArray>"
                                  "<bondArray/>"
                                "</molecule>"
                                "\n");
const QString MIME_BOND(
    "<bond"
    " atomRefs2=\"a2 a1\""
    " type=\"31\""
    " colorR=\"0\""
    " colorG=\"0\""
    " colorB=\"0\""
    " scalingParameter=\"1\""
    " zLevel=\"2\""
    " coordinates=\"-228,27;-193.359,7\"/>");
const QPointF ATOM_COORDS(5,3);
const QPointF OTHER_ATOM_COORDS(-5,3);
const QString ELEMENT("Ca");
const QString OTHER_ELEMENT("Mg");
const QString XPATH_BOND("molecule/bondArray/bond");
const QString MOLECULE_XML("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                           "<molecule name=\"\">"
                           "<atomArray>"
                           "<atom id=\"a1\" elementType=\"\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"5,5\"/>"
                           "<atom id=\"a2\" elementType=\"\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"0,0\"/>"
                           "</atomArray>"
                           "<bondArray"
                           "><bond atomRefs2=\"a2 a1\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"2\" coordinates=\"0,0;5,5\"/>"
                           "</bondArray>"
                           "</molecule>\n");
const QString ALTERNATIVE_MOLECULE_XML("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                       "<molecule name=\"\">"
                                       "<atomArray>"
                                       "<atom id=\"a1\" elementType=\"\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"0,0\"/>"
                                       "<atom id=\"a2\" elementType=\"\" userCharge=\"0\" disableHydrogens=\"0\" hydrogens=\"0\" shapeType=\"0\" hydrogenAlignment=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"3\" coordinates=\"5,5\"/>"
                                       "</atomArray>"
                                       "<bondArray"
                                       "><bond atomRefs2=\"a1 a2\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" zLevel=\"2\" coordinates=\"0,0;5,5\"/>"
                                       "</bondArray>"
                                       "</molecule>\n");
const QString EMPTY_MOLECULE("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                            "<molecule name=\"\">"
                            "</molecule>");

using XmlAssert::assertThat;

class ClipboardTest : public CxxTest::TestSuite {
  MolScene *scene;

  void pasteFromClipboard(const QString &content) {
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MIME_TYPE, content.toUtf8());
    QApplication::clipboard()->setMimeData(mimeData);
    scene->paste();
  }

  Atom *findAtom(const QString &element) {
    auto atoms = findByType<Atom>(scene->items());
    QList<Atom*> matchingAtoms;
    std::copy_if(atoms.cbegin(), atoms.cend(), std::back_inserter(matchingAtoms), [&] (Atom* a) {return a->element() == element;});
    QSM_ASSERT_EQUALS("Atoms for element '" + element + "'", matchingAtoms.size(), 1);
    return matchingAtoms.size() == 1 ? matchingAtoms.first() : nullptr;
  }
public:
  void setUp() override {
    scene = new MolScene;
  }

  void tearDown() override {
    delete scene;
  }

  void testCopyingAtomMoleculeIsIncluded() {
    auto atom = new Atom(ATOM_COORDS, ELEMENT);
    auto molecule = new Molecule{QSet<Atom*>{atom}, {}};
    scene->addItem(molecule);
    atom->setSelected(true);
    scene->copy();
    QS_ASSERT_EQUALS(QApplication::clipboard()->mimeData()->data(MIME_TYPE), CLIPBOARD_CONTENT);
    TS_ASSERT_EQUALS(scene->items().size(), 2);
    TS_ASSERT_EQUALS(scene->stack()->count(), 0);
  }

  void testCopyingBondIncludesBothAtomsAndMolecule() {
    auto atomA = new Atom(ATOM_COORDS, ELEMENT);
    auto atomB = new Atom(OTHER_ATOM_COORDS, OTHER_ELEMENT);
    auto bond = new Bond(atomA, atomB);
    auto molecule = new Molecule({atomA, atomB}, {bond});
    scene->addItem(molecule);
    bond->setSelected(true);
    scene->copy();
    auto clipboardContent = QApplication::clipboard()->mimeData()->data(MIME_TYPE);
    assertThat(clipboardContent)->hasNodes("molecule/atomArray/atom")->haveAttribute("elementType")
        ->exactlyInAnyOrder({ELEMENT, OTHER_ELEMENT});
    assertThat(clipboardContent)->hasNodes(XPATH_BOND)->exactlyOne();
  }

  void testCopyingMultipleBondsIncludesAllAtomsAndAllBondsBetweenAtoms() {
    auto atomA = new Atom(ATOM_COORDS, "A");
    auto atomB = new Atom(-ATOM_COORDS, "B");
    auto atomC = new Atom(OTHER_ATOM_COORDS, "C");
    auto atomD = new Atom(-OTHER_ATOM_COORDS, "D");
    auto bondAB = new Bond(atomA, atomB);
    auto bondBC = new Bond(atomB, atomC);
    auto bondCD = new Bond(atomC, atomD);
    auto molecule = new Molecule({atomA, atomB, atomC, atomD}, {bondAB, bondBC, bondCD});
    scene->addItem(molecule);
    bondAB->setSelected(true);
    bondCD->setSelected(true);
    scene->copy();

    auto clipboardContent = QApplication::clipboard()->mimeData()->data(MIME_TYPE);
    assertThat(clipboardContent)->hasNodes("molecule/atomArray/atom")->haveAttribute("elementType")
        ->exactlyInAnyOrder({"A", "B", "C", "D"});
    assertThat(clipboardContent)->hasNodes(XPATH_BOND)->times(3);
  }

  void testCopyingBondAndAtomIncludesAtomsOfBondAndAtomAndAllBondsBetweenAtoms() {
    auto atomA = new Atom(ATOM_COORDS, "A");
    auto atomB = new Atom(-ATOM_COORDS, "B");
    auto atomC = new Atom(OTHER_ATOM_COORDS, "C");
    auto atomD = new Atom(-OTHER_ATOM_COORDS, "D");
    auto bondAB = new Bond(atomA, atomB);
    auto bondBC = new Bond(atomB, atomC);
    auto bondCD = new Bond(atomC, atomD);
    auto molecule = new Molecule({atomA, atomB, atomC, atomD}, {bondAB, bondBC, bondCD});
    scene->addItem(molecule);
    bondAB->setSelected(true);
    atomC->setSelected(true);
    scene->copy();

    auto clipboardContent = QApplication::clipboard()->mimeData()->data(MIME_TYPE);
    assertThat(clipboardContent)->hasNodes("molecule/atomArray/atom")->haveAttribute("elementType")
        ->exactlyInAnyOrder({"A", "B", "C"});
    assertThat(clipboardContent)->hasNodes(XPATH_BOND)->times(2);
  }

  void testCopyingAtomAndBondFromDifferentMolecules() {
    auto atomA = new Atom(ATOM_COORDS, "A");
    auto atomB = new Atom(-ATOM_COORDS, "B");
    auto atomC = new Atom(OTHER_ATOM_COORDS, "C");
    auto atomD = new Atom(-OTHER_ATOM_COORDS, "D");
    auto bondAB = new Bond(atomA, atomB);
    auto bondCD = new Bond(atomC, atomD);
    auto moleculeAB = new Molecule({atomA, atomB}, {bondAB});
    auto moleculeCD = new Molecule({atomC, atomD}, {bondCD});
    scene->addItem(moleculeAB);
    scene->addItem(moleculeCD);
    bondAB->setSelected(true);
    atomC->setSelected(true);
    scene->copy();

    auto clipboardContent = QApplication::clipboard()->mimeData()->data(MIME_TYPE);
    assertThat(clipboardContent)->hasNodes("molsketchItems/molecule/atomArray/atom")->haveAttribute("elementType")
        ->exactlyInAnyOrder({"A", "B", "C"});
    assertThat(clipboardContent)->hasNodes("molsketchItems/molecule")->times(2);
    // TODO assert that C is in other molecule
    assertThat(clipboardContent)->hasNodes("molsketchItems/molecule/bondArray/bond")->exactlyOne();
  }

  void testCopyingIncludesPngAndSvgFormats() {
    Atom *atom = new Atom(ATOM_COORDS, ELEMENT);
    scene->addItem(new Molecule{QSet<Atom*>{atom}, {}});
    atom->setSelected(true);
    scene->copy();
    auto actualFormatList = QApplication::clipboard()->mimeData()->formats();
    auto actualFormats = QSet<QString>{actualFormatList.cbegin(), actualFormatList.cend()};
    auto expectedFormats = QSet<QString>{MIME_TYPE, "application/x-qt-image", "image/svg+xml"};
    QS_ASSERT_EQUALS(actualFormats, expectedFormats)
  }

  void testCutting() {
    Atom *atom = new Atom(ATOM_COORDS, ELEMENT);
    scene->addItem(new Molecule{QSet<Atom*>{atom}, {}});
    atom->setSelected(true);
    scene->cut();
    QS_ASSERT_EQUALS(QApplication::clipboard()->mimeData()->data(MIME_TYPE), CLIPBOARD_CONTENT);
    TS_ASSERT_EQUALS(scene->items().size(), 0);
    TS_ASSERT_EQUALS(scene->stack()->count(), 1);
  }

  void testCuttingBond_leavesAtoms_splitsMolecule() {
    auto atomA = new Atom(ATOM_COORDS, "A");
    auto atomB = new Atom(-ATOM_COORDS, "B");
    auto atomC = new Atom(OTHER_ATOM_COORDS, "C");
    auto atomD = new Atom(-OTHER_ATOM_COORDS, "D");
    auto bondAB = new Bond(atomA, atomB);
    auto bondBC = new Bond(atomB, atomC);
    auto bondCD = new Bond(atomC, atomD);
    auto molecule = new Molecule({atomA, atomB, atomC, atomD}, {bondAB, bondBC, bondCD});
    scene->addItem(molecule);
    bondBC->setSelected(true);

    scene->cut();

    auto newA = findAtom("A");
    auto newB = findAtom("B");
    auto newC = findAtom("C");
    auto newD = findAtom("D");

    TS_ASSERT_EQUALS(findByType<Bond>(scene->items()).size(), 2);
    if (newA && newB && newC && newD) {
      TS_ASSERT_EQUALS(newA->molecule(), newB->molecule());
      TS_ASSERT_DIFFERS(newB->molecule(), newC->molecule());
      TS_ASSERT_EQUALS(newC->molecule(), newD->molecule());
      TS_ASSERT_EQUALS(scene->stack()->count(), 1);
    }
  }

  void testCuttingBondOfMoleculeInFrame_leavesTwoMoleculesInFrame() {
    auto atomA = new Atom(ATOM_COORDS, "A");
    auto atomB = new Atom(-ATOM_COORDS, "B");
    auto bondAB = new Bond(atomA, atomB);
    auto frame = new Frame();
    new Molecule({atomA, atomB}, {bondAB}, frame);
    scene->addItem(frame);
    bondAB->setSelected(true);

    scene->debugScene();
    scene->cut();
    scene->debugScene();

    auto newA = findAtom("A");
    auto newB = findAtom("B");
    if (newA && newB) {
      TS_ASSERT_DIFFERS(newA->molecule(), newB->molecule());
      TS_ASSERT(newA->molecule());
      TS_ASSERT(newB->molecule());
      TS_ASSERT_EQUALS(newA->molecule()->parentItem(), frame);
      TS_ASSERT_EQUALS(newB->molecule()->parentItem(), frame);
    } else {
      TSM_ASSERT("Atom A could not be found after operation", newA);
      TSM_ASSERT("Atom B could not be found after operation", newB);
    }
  }

  void testCuttingBondOfMoleculeInFrame_undoRestoresFrameAsParent() {
    auto atomA = new Atom(ATOM_COORDS, "A");
    auto atomB = new Atom(-ATOM_COORDS, "B");
    auto bondAB = new Bond(atomA, atomB);
    auto frame = new Frame();
    auto molecule = new Molecule({atomA, atomB}, {bondAB}, frame);
    scene->addItem(frame);
    bondAB->setSelected(true);

    // TODO idea: scene debug test (inherit debugging functionality from parent)
    //       or: in debug mode always debug scene state on change of undo index
    scene->debugScene();
    scene->cut();
    scene->stack()->undo();
    scene->debugScene();

    TS_ASSERT_EQUALS(scene->items().size(), 5);
    TS_ASSERT_EQUALS(atomA->neighbours(), QList<Atom*>{atomB});
    TS_ASSERT_EQUALS(atomA->molecule(), molecule);
    TS_ASSERT_EQUALS(bondAB->molecule(), molecule);
    TS_ASSERT_EQUALS(molecule->parentItem(), frame);
  }

  void testCuttingAtomFromMolecule_cleansMoleculeFromScene() {
    auto atom = new Atom(ATOM_COORDS, "A");
    auto molecule = new Molecule(QSet<Atom*>{atom}, {});
    scene->addItem(molecule);
    atom->setSelected(true);

    scene->cut();

    TS_ASSERT_EQUALS(scene->items().size(), 0);
    TS_ASSERT_EQUALS(scene->stack()->count(), 1);
  }

  void testPasting() {
    pasteFromClipboard(CLIPBOARD_CONTENT);
    TS_ASSERT_EQUALS(scene->items().size(), 2);
    QS_ASSERT_ON_POINTER(Atom, scene->items().first(), element(), ELEMENT);
    TS_ASSERT_EQUALS(scene->stack()->count(), 1);
  }

  void testPastingSingleAtom_wrapsInMolecule() {
    pasteFromClipboard(MIME_ATOM);
    TS_ASSERT_EQUALS(scene->items().size(), 2);
    QS_ASSERT_ON_POINTER(Atom, scene->items().first(), element(), ELEMENT);
    TS_ASSERT_EQUALS(scene->stack()->count(), 1);
  }

  void testPastingLoneBond_isIgnored() {
    pasteFromClipboard(MIME_BOND);
    TS_ASSERT(scene->items().empty());
    TS_ASSERT_EQUALS(scene->stack()->count(), 0);
  }


  void testCopyingMolecules() {
    auto atomA = new Atom;
    auto atomB = new Atom(QPointF(5,5));
    auto bond = new Bond(atomA, atomB);
    auto molecule = new Molecule({atomA, atomB}, {bond});
    scene->addItem(molecule);
    scene->selectAll();
    scene->copy();
    auto mimeData = QApplication::clipboard()->mimeData();
    QS_ASSERT_EQUALS(toSet(mimeData->formats()), QSet<QString>() << "application/x-qt-image" << "molecule/molsketch" << "image/svg+xml");
    QS_ASSERT_EQUALS_OR_EQUALS(mimeData->data("molecule/molsketch"), MOLECULE_XML, ALTERNATIVE_MOLECULE_XML);
  }

  void testPastingMolecules() {
    auto mimeData = new QMimeData;
    mimeData->setData("molecule/molsketch", MOLECULE_XML.toUtf8());
    QApplication::clipboard()->setMimeData(mimeData);
    scene->paste();
    QS_ASSERT_EQUALS(scene->items().size(), 4);
  }

  void testPastingEmptyMolecule_getsCleanedAway() {
    pasteFromClipboard(EMPTY_MOLECULE);
    TS_ASSERT(scene->items().empty());
    TS_ASSERT_EQUALS(scene->stack()->count(), 1);
  }

  // TODO check what happens if atom and molecule are selected
  // TODO test cases without selection/clipboard content
};
