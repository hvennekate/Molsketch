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

#ifndef ELEMENTALIGNMENT_H
#define ELEMENTALIGNMENT_H

#include <QWidget>
#include "neighboralignment.h"

namespace Molsketch {
  class ElementAlignmentPrivate;

  class ElementAlignment : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ElementAlignment)
    QScopedPointer<ElementAlignmentPrivate> d_ptr;
  public:
    explicit ElementAlignment(QWidget *parent = nullptr);
    ~ElementAlignment();
    NeighborAlignment getAlignment() const;
  signals:
    void alignmentChanged(const NeighborAlignment &newAlignment);
  public slots:
    void setAlignment(const NeighborAlignment &newAlignment);
    void setElement(const QString &elementSymbol);
  private slots:
    void on_automatic_toggled(bool on);
    void on_east_toggled(bool on);
    void on_west_toggled(bool on);
    void on_north_toggled(bool on);
    void on_south_toggled(bool on);
  };
} // namespace

#endif // ELEMENTALIGNMENT_H
