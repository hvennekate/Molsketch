/***************************************************************************
 *   Copyright (C) 2020 Hendrik Vennekate                                  *
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

#include <QAbstractButton>
#include <QMutex>
#include <QMap>

#include "elementalignment.h"
#include "ui_elementalignment.h"

namespace Molsketch {

  struct ElementAlignmentPrivate{
    Ui::ElementAlignment *ui;
    QMap<NeighborAlignment, QAbstractButton*> mapping;
    QMutex lock;
  };

  ElementAlignment::ElementAlignment(QWidget *parent)
    : QWidget(parent),
      d_ptr(new ElementAlignmentPrivate)
  {
    Q_D(ElementAlignment);
    d->ui = new Ui::ElementAlignment;
    d->ui->setupUi(this);
    d->mapping[north] = d->ui->north;
    d->mapping[south] = d->ui->south;
    d->mapping[west] = d->ui->west;
    d->mapping[east] = d->ui->east;
    d->mapping[automatic] = d->ui->automatic;
  }

  ElementAlignment::~ElementAlignment() {}

  NeighborAlignment ElementAlignment::getAlignment() const {
    Q_D(const ElementAlignment);
    auto it = d->mapping.begin();
    while (it != d->mapping.end()) {
      if (it.value()->isChecked()) return it.key();
    }
    return automatic;
  }

  void ElementAlignment::setAlignment(const NeighborAlignment &newAlignment) {
    Q_D(ElementAlignment);
    d->mapping[newAlignment]->setChecked(true);
  }

  void ElementAlignment::setElement(const QString &elementSymbol) {
    Q_D(ElementAlignment);
    d->ui->element->setText(elementSymbol);
  }

  void ElementAlignment::on_automatic_toggled(bool on) {
    if (on) emit alignmentChanged(automatic);
  }

  void ElementAlignment::on_east_toggled(bool on) {
    if (on) emit alignmentChanged(east);
  }

  void ElementAlignment::on_west_toggled(bool on) {
    if (on) emit alignmentChanged(west);
  }

  void ElementAlignment::on_north_toggled(bool on) {
    if (on) emit alignmentChanged(north);
  }

  void ElementAlignment::on_south_toggled(bool on) {
    if (on) emit alignmentChanged(south);
  }
} // namespace
