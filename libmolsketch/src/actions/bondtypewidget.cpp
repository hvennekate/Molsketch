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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "bondtypewidget.h"
#include <QButtonGroup>
#include <QToolButton>
#include <QHBoxLayout>

namespace Molsketch {

#define ADDBONDBUTTON(TYPE, ICON, INVERTED) addButton(TYPE, QPixmap::fromImage(QImage(":images/" ICON ".svg").mirrored(INVERTED, INVERTED)));

  bondTypeWidget::bondTypeWidget(bool withInversion, QWidget *parent)
    : ItemTypeWidget(parent)
  {
    ADDBONDBUTTON(Bond::Single, "single", false)
    ADDBONDBUTTON(Bond::Hash, "hash", false)
    if (withInversion) ADDBONDBUTTON(- Bond::Hash, "hash", true)
    ADDBONDBUTTON(Bond::Wedge, "wedge", false)
    if (withInversion) ADDBONDBUTTON(- Bond::Wedge, "wedge", true)
    ADDBONDBUTTON(Bond::WedgeOrHash, "hashOrWedge", false)
    ADDBONDBUTTON(Bond::Double, "double", false)
    ADDBONDBUTTON(Bond::CisOrTrans, "cistrans", false)
    ADDBONDBUTTON(Bond::Triple, "triple", false)
  }

  bool bondTypeWidget::backward() const
  {
    return fullType() < 0 ;
  }

  Bond::BondType bondTypeWidget::bondType() const
  {
    return (Bond::BondType) currentType() ;
  }

  void bondTypeWidget::setBondType(Bond::BondType type)
  {
    setCurrentType(type);
  }

} // namespace
