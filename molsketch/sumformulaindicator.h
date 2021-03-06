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
#ifndef SUMFORMULAINDICATOR_H
#define SUMFORMULAINDICATOR_H

#include <QLabel>

namespace Molsketch {
  class MolScene;
}

class SumFormulaIndicator : public QLabel
{
  Q_OBJECT
private:
  Molsketch::MolScene *scene;
public:
  explicit SumFormulaIndicator(Molsketch::MolScene *scene, QWidget *parent = nullptr);
public slots:
  void updateSumFormula();
};

#endif // SUMFORMULAINDICATOR_H
