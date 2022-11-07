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

#include "utilities.h"
#include <QGraphicsItem>
#include "radicalelectron.h"
#include "painting/textline.h"
namespace Molsketch {
  class Molecule;
  class MoleculeModelItem;
  class LibraryModel;
  class PiElectrons;
  class TextBox;
}
template<> int ForTesting<Molsketch::Molecule>::instanceCounter = 0;
template<> int ForTesting<Molsketch::MoleculeModelItem>::instanceCounter = 0;
template<> int ForTesting<Molsketch::LibraryModel>::instanceCounter = 0;
template<> int ForTesting<QGraphicsItem>::instanceCounter = 0;
template<> int ForTesting<Molsketch::RadicalElectron>::instanceCounter = 0;
template<> int ForTesting<Molsketch::PiElectrons>::instanceCounter = 0;
template<> int ForTesting<Molsketch::TextBox>::instanceCounter = 0;
template<> int ForTesting<Molsketch::TextLine>::instanceCounter = 0;
