/***************************************************************************
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
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
#include "atompopup.h"
#include "ui_atompopup.h"
#include "coordinatemodel.h"
#include "commands.h"
#include "molscene.h"
#include "scenesettings.h"
#include "settingsitem.h"
#include "atom.h"
#include "molscene.h"
#include "radicalelectron.h"
#include "lonepair.h"
#include <QDebug>

namespace Molsketch {

  struct AtomPopup::PrivateData
  {
    Atom* atom;
    Ui::AtomPopup* ui;

    template<class T>
    QVector<T*> childrenOfAtom() {
      QList<QGraphicsItem*> childItems = atom->childItems();
      QVector<T*> radicals(childItems.size());
      std::transform(childItems.begin(), childItems.end(), radicals.begin(), [](QGraphicsItem* item) { return dynamic_cast<T*>(item);});
      radicals.removeAll(nullptr);
      return radicals;
    }

    void getFromAtom() {
      ui->element->setText(atom->element());
      ui->shapeType->setCurrentIndex(atom->shapeType());
      ui->charge->setValue(atom->charge());
      ui->hydrogens->setValue(atom->numImplicitHydrogens());
      ui->coordinates->model()->setCoordinates(atom->coordinates());
      ui->newmanDiameter->setValue(atom->getNewmanDiameter());
      getRadicalsFromAtom();
      getLonePairsFromAtom();
      ui->coordinates->resizeRowsToContents();
      ui->hydrogenAlignment->setAlignment(atom->hAlignment());
    }

    void getRadicalsFromAtom() {
      MolScene *scene = dynamic_cast<MolScene*>(atom->scene());
      qreal defaultRadicalDiameter = scene ?  scene->settings()->radicalDiameter()->get() : 1.5;
      auto radicals = childrenOfAtom<RadicalElectron>();
      qreal sumOfDiameters = 0;
      std::for_each(radicals.begin(), radicals.end(), [&](const RadicalElectron* radical) { sumOfDiameters += radical->diameter();});
      ui->radicalDiameter->setValue(radicals.isEmpty() ? defaultRadicalDiameter : (sumOfDiameters / radicals.size()));

      QVector<BoundingBoxLinker> radicalPositions(radicals.size());
      std::transform(radicals.begin(), radicals.end(), radicalPositions.begin(), [](const RadicalElectron* radical) {return radical->linker();});
      ui->topLeftRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::upperLeft())); // TODO link checkbox to linker
      ui->topRightRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::upperRight()));
      ui->bottomLeftRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::lowerLeft()));
      ui->bottomRightRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::lowerRight()));
      ui->topRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::above()));
      ui->bottomRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::below()));
      ui->leftRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::toLeft()));
      ui->rightRadical->setChecked(radicalPositions.contains(BoundingBoxLinker::toRight()));
    }

    void getLonePairsFromAtom() {
      MolScene *scene = dynamic_cast<MolScene*>(atom->scene());
      qreal defaultLonePairLength = scene ?  scene->settings()->lonePairLength()->get() : 5;
      qreal defaultLonePairLineWidth = scene ? scene->settings()->lonePairLineWidth()->get() : 1;
      auto lonePairs = childrenOfAtom<LonePair>();
      qreal sumOfLineWidths = 0, sumOfLengths = 0;
      std::for_each(lonePairs.begin(), lonePairs.end(), [&] (const LonePair* lonePair) { sumOfLengths += lonePair->length();
      sumOfLineWidths += lonePair->lineWidth(); });
      ui->lonePairLength->setValue(lonePairs.empty() ? defaultLonePairLength : (sumOfLengths / lonePairs.size()));
      ui->lonePairLineWidth->setValue(lonePairs.empty() ? defaultLonePairLineWidth : (sumOfLineWidths / lonePairs.size()));

      QVector<BoundingBoxLinker> lonePairPositions(lonePairs.size());
      std::transform(lonePairs.begin(), lonePairs.end(), lonePairPositions.begin(), [](const LonePair* lonePair) { return lonePair->linker();});
      ui->topLeftLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atTopLeft()));
      ui->topLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atTop()));
      ui->topRightLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atTopRight()));
      ui->bottomLeftLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atBottomLeft()));
      ui->bottomLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atBottom()));
      ui->bottomRightLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atBottomRight()));
      ui->leftLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atLeft()));
      ui->rightLonePair->setChecked(lonePairPositions.contains(BoundingBoxLinker::atRight()));
    }
  };

  AtomPopup::AtomPopup(QWidget *parent) :
    PropertiesWidget(parent),
    ui(new Ui::AtomPopup),
    d(new PrivateData)
  {
    ui->setupUi(this);
    d->ui = ui;
    d->atom = 0;
    connect(ui->coordinates->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(onCoordinatesDatachanged()));
    setObjectName("atom properties");
  }

  AtomPopup::~AtomPopup() {
    delete ui;
    delete d;
  }

  void AtomPopup::connectAtom(Atom *a) {
    d->atom = a;
    setScene(a ? dynamic_cast<MolScene*>(a->scene()) : 0);
  }

  void AtomPopup::on_element_textChanged(const QString &newElementSymbol) {
    attemptToPushUndoCommand(new Commands::ChangeElement(d->atom, newElementSymbol, tr("Change element")));
  }

  void AtomPopup::on_charge_valueChanged(int newCharge) {
    attemptToPushUndoCommand(new Commands::setAtomChargeCommand(d->atom, newCharge, tr("Change charge")));
  }

  void AtomPopup::on_hydrogens_valueChanged(int newHydrogenCount) {
    attemptToPushUndoCommand(new Commands::setImplicitHydrogensCommand(d->atom, newHydrogenCount, tr("Change number of hydrogens")));
  }

  void AtomPopup::on_newmanDiameter_valueChanged(double diameter) {
    attemptToPushUndoCommand(new Commands::SetNewmanDiameter(d->atom, diameter, tr("Change Newman diameter")));
  }

  void AtomPopup::on_shapeType_currentIndexChanged(int newIndex) {
    attemptToPushUndoCommand(new Commands::SetShapeType(d->atom, (Atom::ShapeType) newIndex));
  }

  void AtomPopup::onCoordinatesDatachanged() {
    if (!d->atom) return;
    attemptToPushUndoCommand(Commands::MoveItem::absolute(d->atom, ui->coordinates->model()->getCoordinates().first()));
  }

  void AtomPopup::updateRadicals() {
    if (!d->atom) return;
    attemptBeginMacro(tr("Change radical electrons"));
    for (RadicalElectron* child : d->childrenOfAtom<RadicalElectron>())
      attemptToPushUndoCommand(new Commands::ChildItemCommand(d->atom, child));
    addRadical(ui->topLeftRadical, BoundingBoxLinker::upperLeft());
    addRadical(ui->topRightRadical, BoundingBoxLinker::upperRight());
    addRadical(ui->bottomLeftRadical, BoundingBoxLinker::lowerLeft());
    addRadical(ui->bottomRightRadical, BoundingBoxLinker::lowerRight());
    addRadical(ui->topRadical, BoundingBoxLinker::above());
    addRadical(ui->bottomRadical, BoundingBoxLinker::below());
    addRadical(ui->leftRadical, BoundingBoxLinker::toLeft());
    addRadical(ui->rightRadical, BoundingBoxLinker::toRight());
    attemptEndMacro();
  }

  void AtomPopup::updateLonePairs()
  {
    if (!d->atom) return;
    attemptBeginMacro(tr("Change lone pairs"));
    for (LonePair *child : d->childrenOfAtom<LonePair>())
      attemptToPushUndoCommand(new Commands::ChildItemCommand(d->atom, child));
    addLonePair(ui->topLeftLonePair, BoundingBoxLinker::atTopLeft(), 45);
    addLonePair(ui->topRightLonePair, BoundingBoxLinker::atTopRight(), 315);
    addLonePair(ui->bottomLeftLonePair, BoundingBoxLinker::atBottomLeft(), 135);
    addLonePair(ui->bottomRightLonePair, BoundingBoxLinker::atBottomRight(), 225);
    addLonePair(ui->topLonePair, BoundingBoxLinker::atTop(), 0);
    addLonePair(ui->bottomLonePair, BoundingBoxLinker::atBottom(), 180);
    addLonePair(ui->leftLonePair, BoundingBoxLinker::atLeft(), 90);
    addLonePair(ui->rightLonePair, BoundingBoxLinker::atRight(), 270);
    attemptEndMacro();
  }

  void AtomPopup::updateHAlignment(const NeighborAlignment &newAlignment) {
    if (!d->atom) return;
    attemptToPushUndoCommand(new Commands::SetHAlignment(d->atom, newAlignment));
  }

  void AtomPopup::propertiesChanged()
  {
    if (!scene()) return; // TODO reset GUI
    if (itemValid(d->atom)) d->getFromAtom();
  }

  void AtomPopup::addRadical(const QCheckBox* checkBox, const BoundingBoxLinker &linker) {
    if (!checkBox->isChecked()) return;
    attemptToPushUndoCommand(new Commands::ChildItemCommand(d->atom, new RadicalElectron(ui->radicalDiameter->value(), linker)));
  }

  void AtomPopup::addLonePair(const QCheckBox *checkBox, const BoundingBoxLinker &linker, const qreal angle) {
    if (!checkBox->isChecked()) return;
    attemptToPushUndoCommand(new Commands::ChildItemCommand(d->atom, new LonePair(angle, ui->lonePairLineWidth->value(), ui->lonePairLength->value(), linker)));
  }
} // namespace
