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

#include <QButtonGroup>
#include <QToolButton>

#include "itemtypewidget.h"
#include "generics.h"

#define BUTTONPROPERTY "ButtonDataProperty"

namespace Molsketch {

  struct ItemTypeWidget::privateData
  {
    QButtonGroup *buttonGroup ;
    QBoxLayout *layout ;
    ItemTypeWidget *parent;

    void createButton(const int& type,
                      const QPixmap& Icon,
                      const QVariant& data)
    {
      QToolButton *button = new QToolButton(parent) ;
      buttonGroup->addButton(button, type) ;
      button->setIcon(Icon) ;
      button->setProperty(BUTTONPROPERTY, data);
      button->setAutoRaise(true);
      button->setCheckable(true);
      layout->addWidget(button);
      layout->setContentsMargins(0, 0, 0, 0);
    }

    privateData(ItemTypeWidget* p)
      : buttonGroup(new QButtonGroup(p)),
        layout(new QHBoxLayout(p)),
        parent(p)
    {}
  };

  ItemTypeWidget::ItemTypeWidget(QWidget *parent)
    : QWidget(parent),
      d(new privateData(this))
  {
    connect(d->buttonGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(currentTypeChanged(int)));
    d->buttonGroup->setExclusive(true);
  }

  ItemTypeWidget::~ItemTypeWidget()
  {
    delete d;
  }

  uint ItemTypeWidget::currentType() const
  {
    return qAbs(fullType());
  }

  QVariant ItemTypeWidget::currentData() const
  {
    QAbstractButton *currentButton = d->buttonGroup->checkedButton();
    if (!currentButton) return QVariant();
    return currentButton->property(BUTTONPROPERTY);
  }

  void ItemTypeWidget::setCurrentType(const uint &type)
  {
    setFullType(type);
  }

  void ItemTypeWidget::setCurrentType(const QVariant &variant)
  {
    for(auto button : d->buttonGroup->buttons())
      if (button->property(BUTTONPROPERTY) == variant)
        button->setChecked(true);
  }

  int ItemTypeWidget::fullType() const
  {
    return d->buttonGroup->checkedId();
  }

  void ItemTypeWidget::setFullType(const int &type) const
  {
    QAbstractButton *button = d->buttonGroup->button(type);
    if (button) button->setChecked(true);
  }

  QPixmap ItemTypeWidget::currentIcon() const
  {
    return d->buttonGroup->checkedButton()->icon().pixmap(d->buttonGroup->checkedButton()->size());
  }

  void ItemTypeWidget::setDirection(QBoxLayout::Direction dir)
  {
    d->layout->setDirection(dir);
  }

  void ItemTypeWidget::addButton(const int &type, const QPixmap &icon, const QVariant &data)
  {
    d->createButton(type, icon, data);
    if (d->buttonGroup->buttons().size() == 1)
      d->buttonGroup->buttons().first()->setChecked(true);
  }

  void ItemTypeWidget::cycleTypes(bool inverse) // TODO make generic function with multiAction::cycleSubActions
  {
    cycleButtons(d->buttonGroup, inverse);
    emit currentTypeChanged(currentType());
  }

} // namespace Molsketch

