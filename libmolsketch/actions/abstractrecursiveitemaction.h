/***************************************************************************
 *   Copyright (C) 2007 by Harm van Eersel                                 *
 *   devsciurus@xs4all.nl                                                  *
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

/** @file
 * This file is part of molsKetch and contains the abstractItemAction class.
 *
 * This class provides the infrastructure for an action that operates on
 * one or more items.
 *
 * @author Hendrik Vennekate <Hendrik.Vennekate@posteo.de>
 * @since Lithium
 */

#ifndef ABSTRACTRECURSIVEITEMACTION_H
#define ABSTRACTRECURSIVEITEMACTION_H

#include "abstractitemaction.h"
#include <QList>

namespace Molsketch
{
  class abstractRecursiveItemAction : public AbstractItemAction
  {
    Q_OBJECT
  public:
    explicit abstractRecursiveItemAction(MolScene *parent = 0);
  protected:
    QSet<graphicsItem*> filterItems(const QList<QGraphicsItem *> &list) const override;
  };

} // namespace

#define ITERATEOVERITEMSMACRO(COMMENT,COMMAND,VALUE) \
  undoStack()->beginMacro(tr(COMMENT)) ;\
  foreach(graphicsItem* item, items())\
  undoStack()->push(new Commands::COMMAND(item, VALUE)) ;\
  undoStack()->endMacro() ;
#endif // ABSTRACTITEMACTION_H
