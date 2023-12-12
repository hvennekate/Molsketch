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
#ifndef MOLSKETCH_MECHANISMARROWACTION_H
#define MOLSKETCH_MECHANISMARROWACTION_H

#include "reactionarrowaction.h"



namespace Molsketch {

  class mechanismArrowAction : public reactionArrowAction {
    Q_OBJECT
  public:
    explicit mechanismArrowAction(MolScene* scene);
    ~mechanismArrowAction();
  private:
    QPolygonF makePolygon(const QLineF &line) override;
  };

} // namespace Molsketch

#endif // MOLSKETCH_MECHANISMARROWACTION_H
