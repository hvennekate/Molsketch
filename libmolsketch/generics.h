/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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

#ifndef GENERICS
#define GENERICS

#include <QAbstractButton>
#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QList>


template <class itemType,
          void (itemType::*checkItemFunction)(bool),
          class containerType,
          QList<itemType*> (containerType::*getItemsFunction)() const,
          itemType* (containerType::*getCurrentItemFunction)() const
          >
void cycleItems(containerType* container, bool reverse = false)
{
  QList<itemType*> subItems((container->*getItemsFunction)());
  if (subItems.size() <= 1) return;
  itemType *activeItem = (container->*getCurrentItemFunction)();
  if (!activeItem) return ;
  int index = subItems.indexOf(activeItem);
  if (-1 == index) return;
  index += reverse ? -1 : 1;
  if (index < 0) index = subItems.size()-1;
  if (index == subItems.size()) index = 0;
  (subItems.at(index)->*checkItemFunction)(true);
}

inline void cycleActions(QActionGroup* group, bool reverse = false)
{
  cycleItems<QAction, &QAction::setChecked,
      QActionGroup, &QActionGroup::actions, &QActionGroup::checkedAction>
      (group, reverse);
}

inline void cycleButtons(QButtonGroup* group, bool reverse = false)
{
  cycleItems<QAbstractButton, &QAbstractButton::setChecked,
      QButtonGroup, &QButtonGroup::buttons, &QButtonGroup::checkedButton>
      (group, reverse);
}

#endif // GENERICS

