/***************************************************************************
 *   Copyright (C) 2016 by Hendrik Vennekate                               *
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

#include <QLineEdit>
#include <QSpinBox>
#include <atom.h>
#include <atompopup.h>
#include <coordinatetableview.h>
#include <coordinatemodel.h>
#include <molecule.h>
#include <molscene.h>
#include <commands.h>
#include <molview.h>
#include <scenesettings.h>
#include <settingsitem.h>
#include <QCheckBox>
#include <radicalelectron.h>
#include <lonepair.h>
#include <cxxtest/TestSuite.h>
#include "utilities.h"

using namespace Molsketch;

const qreal RADICAL_DIAMETER = 2.5;
const qreal OTHER_RADICAL_DIAMETER = 1.5;
const qreal LONE_PAIR_ANGLE = 45;
const qreal LONE_PAIR_LINE_WIDTH = 1.5;
const qreal LONE_PAIR_LENGTH = 5.5;
const qreal OTHER_LONE_PAIR_LENGTH = 6.5;
const qreal OTHER_LONE_PAIR_LINE_WIDTH = 2.5;

class AtomPopupUnitTest : public CxxTest::TestSuite {
  Atom *atom;
  AtomPopup *popup;
  QSpinBox *chargeBox;
  QLineEdit *elementEditor;
  QSpinBox *hydrogenBox;
  CoordinateTableView *coordinateTable;
  QDoubleSpinBox *radicalDiameter, *lonePairLength, *lonePairLineWidth;
  MolScene* scene;
  static constexpr char const* initialElement = "Ca";
  void resetAtom() {
    scene->stack()->clear();
    Molecule *molecule = (Molecule*) atom->parentItem();
    molecule->delAtom(atom);
    delete atom;
    atom = new Atom;
    molecule->addAtom(atom);
  }
public:
  void setUp() {
    popup = new AtomPopup;
    scene = new MolScene();
    atom = new Atom;
    Molecule *molecule = new Molecule(QSet<Atom*>() << atom, QSet<Bond*>());
    scene->addItem(molecule);

    elementEditor = popup->findChild<QLineEdit*>("element");
    chargeBox = popup->findChild<QSpinBox*>("charge");
    hydrogenBox = popup->findChild<QSpinBox*>("hydrogens");
    coordinateTable = popup->findChild<CoordinateTableView*>("coordinates");
    radicalDiameter = popup->findChild<QDoubleSpinBox*>("radicalDiameter");
    lonePairLength = popup->findChild<QDoubleSpinBox*>("lonePairLength");
    lonePairLineWidth = popup->findChild<QDoubleSpinBox*>("lonePairLineWidth");
  }

  void tearDown() {
    delete popup;
    delete scene;
  }

  void testGuiElementsFound() {
    TS_ASSERT(elementEditor);
    TS_ASSERT(chargeBox);
    TS_ASSERT(hydrogenBox);
    TS_ASSERT(coordinateTable);
    TS_ASSERT(radicalDiameter);
    TS_ASSERT(lonePairLength);
    TS_ASSERT(lonePairLineWidth);

    TS_ASSERT(!popup->isEnabled());
  }

  void testGuiElementsFilledOnSetup() {
    const char *element = initialElement;
    const QPolygonF coordinates(QPolygonF() << QPointF(30.5,20.5));
    const int charge = -3;
    const int numImplicitHydrogens = 2;

    atom->setElement(element);
    atom->setCoordinates(coordinates);
    atom->setCharge(charge);
    atom->setNumImplicitHydrogens(numImplicitHydrogens);

    popup->connectAtom(atom);

    QS_ASSERT_EQUALS(elementEditor->text(), element);
    QS_ASSERT_EQUALS(chargeBox->value(), charge);
    QS_ASSERT_EQUALS(hydrogenBox->value(), numImplicitHydrogens);
    QS_ASSERT_EQUALS(coordinateTable->model()->getCoordinates(), coordinates);
    QS_ASSERT_EQUALS(radicalDiameter->value(), scene->settings()->radicalDiameter()->get());
    QS_ASSERT_EQUALS(lonePairLength->value(), scene->settings()->lonePairLength()->get());
    QS_ASSERT_EQUALS(lonePairLineWidth->value(), scene->settings()->lonePairLineWidth()->get());
    QS_ASSERT_EQUALS(atom->numImplicitHydrogens(), numImplicitHydrogens); // TODO remove exposition of this to GUI or at least make it transparent
    TS_ASSERT(popup->isEnabled());
  }

  void testElementNameTransferFromCommand() {
    QString newElement("Mg");
    popup->connectAtom(atom);
    scene->stack()->push(new Commands::ChangeElement(atom, newElement));
    QS_ASSERT_EQUALS(elementEditor->text(), newElement);
  }

  void testElementNameTransferFromGui() {
    QString expectedSymbol("Ba");
    popup->connectAtom(atom);
    elementEditor->setText(expectedSymbol);
    QS_ASSERT_EQUALS(atom->element(), expectedSymbol);
  }

  void testSymbolEditingInGuiFromStartPosition() {
    QString expectedSymbol("Ba");
    atom->setElement(initialElement);
    popup->connectAtom(atom);
    QS_ASSERT_EQUALS(elementEditor->text(), initialElement);
    enterTextIntoInputWidget(elementEditor, expectedSymbol);
    QS_ASSERT_EQUALS(atom->element(), expectedSymbol + initialElement);
  }

  void testChargeTransferFromCommad() {
    int newCharge(-5);
    popup->connectAtom(atom);
    scene->stack()->push(new Commands::setAtomChargeCommand(atom, newCharge));
    TS_ASSERT_EQUALS(chargeBox->value(), newCharge);
  }

  void testChargeTransferFromGui() {
    int newCharge = -5;
    popup->connectAtom(atom);
    chargeBox->setValue(newCharge);
    TS_ASSERT_EQUALS(atom->charge(), newCharge);
  }

  void testHydrogenCountTransferFromCommand() {
    int newHydrogenCount(5);
    popup->connectAtom(atom);
    scene->stack()->push(new Commands::setImplicitHydrogensCommand(atom, newHydrogenCount));
    TS_ASSERT_EQUALS(hydrogenBox->value(), newHydrogenCount);
  }

  void testHydrogenCountTransferFromGui() {
    int newHydrogenCount = 5;
    popup->connectAtom(atom);
    hydrogenBox->setValue(newHydrogenCount);
    TS_ASSERT_EQUALS(atom->numImplicitHydrogens(), newHydrogenCount);
  }

  void testCoordinateTransferFromCommand() {
    QPointF originalPosition(5,5), shift(3,4);
    atom->setCoordinate(0, originalPosition);
    popup->connectAtom(atom);
    scene->stack()->push(Commands::MoveItem::relative(atom, shift));
    QVector<QPointF> expectedCoordinates;
    expectedCoordinates << originalPosition + shift;
    QS_ASSERT_EQUALS(coordinateTable->model()->getCoordinates(), expectedCoordinates);
  }

  void testCoordinateTransferFromGui() {
    atom->setCoordinates({QPointF(0,0)});
    popup->connectAtom(atom);
    enterDataIntoCell(coordinateTable, "2.3", 0, 0);
    enterDataIntoCell(coordinateTable, "5.2", 0, 1);
    QVector<QPointF> expectedCoordinates = {QPointF(2.3, 5.2)};
    QS_ASSERT_EQUALS(coordinateTable->model()->getCoordinates(), expectedCoordinates);
    QS_ASSERT_EQUALS(atom->coordinates(), expectedCoordinates);
  }

  void testCoordinateTransferChangeFromScene() {
    atom->setCoordinates({QPointF(0,0)});
    popup->connectAtom(atom);
    MolView view(scene);

    view.show();
    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::NoModifier, view.mapFromScene(QPointF(0,0)));
    QPoint releasePos(view.mapFromScene(QPointF(10,20)));
    mouseMoveEvent(view.viewport(), Qt::LeftButton, Qt::NoModifier, releasePos);
    QVector<QPointF> expectedCoordinates = {QPointF(10,20)};
    QS_ASSERT_EQUALS(atom->coordinates(), expectedCoordinates);
    QS_ASSERT_EQUALS(coordinateTable->model()->getCoordinates(), expectedCoordinates);
  }

  void performRadicalAddition(QString checkBoxName) {
    resetAtom();
    popup->connectAtom(atom);
    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>(checkBoxName)));
    QSM_ASSERT_EQUALS("Check connections for " + checkBoxName, atom->childItems().size(), 1); // TODO make this a hard requirement (throw if fails)
    assertNotNull(dynamic_cast<RadicalElectron*>(atom->childItems().first()));
    QSM_ASSERT_EQUALS(checkBoxName, scene->stack()->count(), 1);
    QSM_ASSERT_EQUALS(checkBoxName, scene->stack()->undoText(), "Change radical electrons");
  }

  void performRadicalAddition(QString checkBoxName, QPointF (QRectF::*atomCorner)() const, QPointF (QRectF::*radicalCorner)() const) {
    performRadicalAddition(checkBoxName);
    QSM_ASSERT_EQUALS(checkBoxName, (atom->childItems().first()->boundingRect().*radicalCorner)(), (atom->boundingRect().*atomCorner)());
  }

  void performRadicalAddition(QString checkBoxName, qreal (QRectF::*atomEdge)() const, qreal (QRectF::*radicalEdge)() const, qreal (QPointF::*centerCoordinate)() const) { // TODO QRectF asserts
    performRadicalAddition(checkBoxName);
    QSM_ASSERT_EQUALS(checkBoxName, (atom->childItems().first()->boundingRect().*radicalEdge)(), (atom->boundingRect().*atomEdge)());
    QSM_ASSERT_EQUALS(checkBoxName, (atom->childItems().first()->boundingRect().center().*centerCoordinate)(), (atom->boundingRect().center().*centerCoordinate)());
  }

  void testRadicalSelection() {
    performRadicalAddition("topLeftRadical", &QRectF::topLeft, &QRectF::bottomRight);
    performRadicalAddition("topRightRadical", &QRectF::topRight, &QRectF::bottomLeft);
    performRadicalAddition("bottomLeftRadical", &QRectF::bottomLeft, &QRectF::topRight);
    performRadicalAddition("bottomRightRadical", &QRectF::bottomRight, &QRectF::topLeft);
    performRadicalAddition("topRadical", &QRectF::top, &QRectF::bottom, &QPointF::x);
    performRadicalAddition("bottomRadical", &QRectF::bottom, &QRectF::top, &QPointF::x);
    performRadicalAddition("leftRadical", &QRectF::left, &QRectF::right, &QPointF::y);
    performRadicalAddition("rightRadical", &QRectF::right, &QRectF::left, &QPointF::y);
  } // TODO test multiple selection

  void checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker linker, const QString& checkBoxName) {
    resetAtom();
    (new RadicalElectron(RADICAL_DIAMETER, linker))->setParentItem(atom);
    popup->connectAtom(atom);
    for (QCheckBox* checkBox : popup->findChildren<QCheckBox*>())
      TSM_ASSERT((checkBox->objectName() + " check state wrong for test of " + checkBoxName).toStdString().data(),
                 checkBox->objectName() == checkBoxName ? checkBox->isChecked() : !checkBox->isChecked());
  }

  void testRadicalConfigurationTransferredToGui() {
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::above(), "topRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::below(), "bottomRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::toLeft(), "leftRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::toRight(), "rightRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::upperLeft(), "topLeftRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::upperRight(), "topRightRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::lowerLeft(), "bottomLeftRadical");
    checkRadicalConfigurationTransferredToCheckBox(BoundingBoxLinker::lowerRight(), "bottomRightRadical");
  }

  void testChangingRadicalWorks() {
    (new RadicalElectron(RADICAL_DIAMETER, BoundingBoxLinker::above()))->setParentItem(atom);
    popup->connectAtom(atom);

    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>("topRadical")));
    TS_ASSERT(atom->childItems().isEmpty());
    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>("topLeftRadical")));
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_DIFFERS(dynamic_cast<RadicalElectron*>(atom->childItems().first()), nullptr);
    QS_ASSERT_EQUALS(dynamic_cast<RadicalElectron*>(atom->childItems().first())->boundingRect().bottomRight(), atom->boundingRect().topLeft());
  }

  void testRadicalDiameterIsShown() {
    (new RadicalElectron(RADICAL_DIAMETER, BoundingBoxLinker::above()))->setParentItem(atom);
    popup->connectAtom(atom);
    TS_ASSERT_EQUALS(assertNotNull(popup->findChild<QDoubleSpinBox*>("radicalDiameter"))->value(), RADICAL_DIAMETER);
  }

  void testRadicalDiameterCanBeChanged() {
    (new RadicalElectron(RADICAL_DIAMETER, BoundingBoxLinker::above()))->setParentItem(atom);
    popup->connectAtom(atom);

    assertNotNull(popup->findChild<QDoubleSpinBox*>("radicalDiameter"))->setValue(OTHER_RADICAL_DIAMETER);
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_EQUALS(assertNotNull(dynamic_cast<RadicalElectron*>(atom->childItems()[0]))->diameter(), OTHER_RADICAL_DIAMETER);
    QS_ASSERT_EQUALS(scene->stack()->count(), 1);
    QS_ASSERT_EQUALS(scene->stack()->undoText(), "Change radical electrons");

  }

  void performLonePairAddition(const QString& checkBoxName,
                               QPointF (QRectF::*atomCorner)() const,
                               qreal angle) {
    resetAtom();
    popup->connectAtom(atom);
    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>(checkBoxName)));
    QSM_ASSERT_EQUALS("Check connections for " + checkBoxName, atom->childItems().size(), 1); // TODO make this a hard requirement (throw if fails)
    assertNotNull(dynamic_cast<LonePair*>(atom->childItems().first()));
    QSM_ASSERT_EQUALS(checkBoxName, scene->stack()->count(), 1);
    QSM_ASSERT_EQUALS(checkBoxName, scene->stack()->undoText(), "Change lone pairs");
    QSM_ASSERT_EQUALS(checkBoxName, atom->childItems().first()->boundingRect().center(), (atom->boundingRect().*atomCorner)());
    TSM_ASSERT_DELTA(checkBoxName, dynamic_cast<LonePair*>(atom->childItems().first())->angle(), angle, 1e-4); // TODO delta
  }

  void performLonePairAddition(const QString& checkBoxName,
                               qreal (QRectF::*atomEdge)() const,
                               qreal (QPointF::*edgeCoordinate)() const,
                               qreal (QPointF::*atomCenterCoordinate)() const,
                               qreal angle) {
    resetAtom();
    popup->connectAtom(atom);
    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>(checkBoxName)));
    QSM_ASSERT_EQUALS("Check connections for " + checkBoxName, atom->childItems().size(), 1); // TODO make this a hard requirement (throw if fails)
    assertNotNull(dynamic_cast<LonePair*>(atom->childItems().first()));
    QSM_ASSERT_EQUALS(checkBoxName, scene->stack()->count(), 1);
    QSM_ASSERT_EQUALS(checkBoxName, scene->stack()->undoText(), "Change lone pairs");
    QSM_ASSERT_DELTA(checkBoxName, (atom->childItems().first()->boundingRect().center().*edgeCoordinate)(), (atom->boundingRect().*atomEdge)(), 10 * std::numeric_limits<double>::epsilon()); // TODO why not exact?
    QSM_ASSERT_EQUALS(checkBoxName, (atom->childItems().first()->boundingRect().center().*atomCenterCoordinate)(), (atom->boundingRect().center().*atomCenterCoordinate)());
    TSM_ASSERT_DELTA(checkBoxName, dynamic_cast<LonePair*>(atom->childItems().first())->angle(), angle, 1e-4); // TODO delta
  }

  void checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker linker, const QString& checkBoxName) {
    resetAtom();
    (new LonePair(LONE_PAIR_ANGLE, LONE_PAIR_LINE_WIDTH, LONE_PAIR_LENGTH, linker))->setParentItem(atom);
    popup->connectAtom(atom);
    for (QCheckBox* checkBox : popup->findChildren<QCheckBox*>())
      TSM_ASSERT((checkBox->objectName() + " check state wrong for test of " + checkBoxName).toStdString().data(),
                 checkBox->objectName() == checkBoxName ? checkBox->isChecked() : !checkBox->isChecked());
  }

  void testLonePairSelection() {
    performLonePairAddition("topLeftLonePair", &QRectF::topLeft, 45);
    performLonePairAddition("topRightLonePair", &QRectF::topRight, 315);
    performLonePairAddition("bottomLeftLonePair", &QRectF::bottomLeft, 135);
    performLonePairAddition("bottomRightLonePair", &QRectF::bottomRight, 225);
    performLonePairAddition("topLonePair", &QRectF::top, &QPointF::y, &QPointF::x, 0);
    performLonePairAddition("bottomLonePair", &QRectF::bottom, &QPointF::y, &QPointF::x, 180);
    performLonePairAddition("leftLonePair", &QRectF::left, &QPointF::x, &QPointF::y, 90);
    performLonePairAddition("rightLonePair", &QRectF::right, &QPointF::x, &QPointF::y, 270);
  }

  void testLonePairConfigurationTransferredToGui() {
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atTop(), "topLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atBottom(), "bottomLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atLeft(), "leftLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atRight(), "rightLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atTopLeft(), "topLeftLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atTopRight(), "topRightLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atBottomLeft(), "bottomLeftLonePair");
    checkLonePairConfigurationTransferredToCheckBox(BoundingBoxLinker::atBottomRight(), "bottomRightLonePair");
  }

  void testChangingLonePairsWorks() {
    (new LonePair(LONE_PAIR_ANGLE, LONE_PAIR_LINE_WIDTH, LONE_PAIR_LENGTH, BoundingBoxLinker::atTop()))->setParentItem(atom);
    popup->connectAtom(atom);

    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>("topLonePair")));
    TS_ASSERT(atom->childItems().isEmpty());
    clickCheckBox(assertNotNull(popup->findChild<QCheckBox*>("topLeftLonePair")));
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_DIFFERS(dynamic_cast<LonePair*>(atom->childItems().first()), nullptr);
    QS_ASSERT_EQUALS(dynamic_cast<LonePair*>(atom->childItems().first())->boundingRect().center(), atom->boundingRect().topLeft());
  }

  void testLonePairLengthAndLineWidthDisplayed() {
    (new LonePair(LONE_PAIR_ANGLE, LONE_PAIR_LINE_WIDTH, LONE_PAIR_LENGTH, BoundingBoxLinker::atTop()))->setParentItem(atom);
    popup->connectAtom(atom);
    TS_ASSERT_EQUALS(assertNotNull(popup->findChild<QDoubleSpinBox*>("lonePairLength"))->value(), LONE_PAIR_LENGTH);
    TS_ASSERT_EQUALS(assertNotNull(popup->findChild<QDoubleSpinBox*>("lonePairLineWidth"))->value(), LONE_PAIR_LINE_WIDTH);
  }

  void testLonePairLengthChangeWorks() {
    (new LonePair(LONE_PAIR_ANGLE, LONE_PAIR_LINE_WIDTH, LONE_PAIR_LENGTH, BoundingBoxLinker::atTop()))->setParentItem(atom);
    popup->connectAtom(atom);

    assertNotNull(popup->findChild<QDoubleSpinBox*>("lonePairLength"))->setValue(OTHER_LONE_PAIR_LENGTH);
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_EQUALS(assertNotNull(dynamic_cast<LonePair*>(atom->childItems()[0]))->length(), OTHER_LONE_PAIR_LENGTH);
    QS_ASSERT_EQUALS(scene->stack()->count(), 1);
    QS_ASSERT_EQUALS(scene->stack()->undoText(), "Change lone pairs");
  }

  void testLonePairLineWidthChangeWorks() {
    (new LonePair(LONE_PAIR_ANGLE, LONE_PAIR_LINE_WIDTH, LONE_PAIR_LENGTH, BoundingBoxLinker::atTop()))->setParentItem(atom);
    popup->connectAtom(atom);

    assertNotNull(popup->findChild<QDoubleSpinBox*>("lonePairLineWidth"))->setValue(OTHER_LONE_PAIR_LINE_WIDTH);
    TS_ASSERT_EQUALS(atom->childItems().size(), 1);
    TS_ASSERT_EQUALS(assertNotNull(dynamic_cast<LonePair*>(atom->childItems()[0]))->lineWidth(), OTHER_LONE_PAIR_LINE_WIDTH);
    QS_ASSERT_EQUALS(scene->stack()->count(), 1);
    QS_ASSERT_EQUALS(scene->stack()->undoText(), "Change lone pairs");
  }

  void testElementSymbolInLonePairAndRadicalWidget() {
    // TODO implement
  }

// TODO when opening edit mode, selection is lost
//  void testElementNameTransferChangeFromScene() {
//    QString expectedSymbol("Ba");
//    atom->setCoordinates({QPointF(0,0)});
//    popup->connectAtom(atom);
//    MolView view(scene);

//    view.show();
//    QTest::mouseDClick(view.viewport(), Qt::LeftButton, Qt::NoModifier, view.mapFromScene(QPointF(0,0)), 2000);
//    QTest::keyClicks(view.viewport(), expectedSymbol, Qt::NoModifier, 2000);
//    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::NoModifier, view.mapFromScene(QPointF(50,50)), 2000);
//    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::NoModifier, view.mapFromScene(QPointF(50,50)), 2000);

//    QS_ASSERT_EQUALS(atom->element(), expectedSymbol);
//    QS_ASSERT_EQUALS(elementEditor->text(), expectedSymbol);
//  }

  // TODO Test deletion of atom from scene.
  // TODO extract parent class from this and arrowPopupAcceptanceTest

  void testNewmanDiameterInitiallyZero() {
    popup->connectAtom(atom);
    QS_ASSERT_ON_POINTER(QDoubleSpinBox, popup->findChild<QDoubleSpinBox*>("newmanDiameter"), value(), 0);
  }

  void testInitialNewmanDiameterIsTransferredToGui() {
    atom->setNewmanDiameter(5.5);
    popup->connectAtom(atom);
    QS_ASSERT_ON_POINTER(QDoubleSpinBox, popup->findChild<QDoubleSpinBox*>("newmanDiameter"), value(), 5.5);
  }

  void testSettingNewmanDiameterWorks() {
    popup->connectAtom(atom);
    assertNotNull(popup->findChild<QDoubleSpinBox*>("newmanDiameter"))->setValue(5.5);
    TS_ASSERT_EQUALS(atom->getNewmanDiameter(), 5.5);
    TS_ASSERT_EQUALS(scene->stack()->count(), 1);
//    QS_ASSERT_EQUALS(scene->stack()->undoText(), "Change Newman diameter");
  }

  void testSettingNewmanDiameterCanBeUndone() {
   popup->connectAtom(atom);
   assertNotNull(popup->findChild<QDoubleSpinBox*>("newmanDiameter"))->setValue(5.5);
   scene->stack()->undo();
   TS_ASSERT_EQUALS(atom->getNewmanDiameter(), 0);
  }
};
