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

#ifndef MOLSKETCH_ITEMGROUPTYPEACTION_H
#define MOLSKETCH_ITEMGROUPTYPEACTION_H

#include "abstractitemaction.h"

namespace Molsketch {

  class ItemTypeWidget;

  class ItemGroupTypeAction : public AbstractItemAction
  {
    Q_OBJECT
  public:
    explicit ItemGroupTypeAction(MolScene *scene = 0);
    ~ItemGroupTypeAction();
  protected:
    void setItemTypeWidget(ItemTypeWidget* widget);
    virtual QString undoName() const;
    virtual void applyType(int type, const QVariant& data) const = 0;
    virtual void getType(int& type, QVariant& data) const = 0;
    virtual int defaultType() const;
    virtual QVariant defaultData() const;
  private:
    class privateData;
    privateData *d;
    void execute() override;
  private slots:
    virtual void checkItemType();
  };

} // namespace Molsketch

#endif // MOLSKETCH_ITEMGROUPTYPEACTION_H
