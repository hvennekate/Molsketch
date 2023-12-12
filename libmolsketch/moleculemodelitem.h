/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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
#ifndef MOLSKETCH_MOLECULEMODELITEM_H
#define MOLSKETCH_MOLECULEMODELITEM_H

#include <QScopedPointer>

class QIcon;
class QString;
class QXmlStreamWriter;
class QPixmap;

namespace Molsketch {
  class Molecule;
  class MoleculeModelItemPrivate;

  /**
   * @brief The MoleculeModelItem class for use in the LibraryModel
   *
   * This class facilitates lazy generation of molecules and model information
   * (icon, XML data) and caching thereof.
   * Subclasses need only implement the produceMolecule() method which shall
   * generate a molecule from which this class will then obtain all further
   * information as the need arises.
   */
  class MoleculeModelItem
  {
    Q_DECLARE_PRIVATE(MoleculeModelItem)
    QScopedPointer<MoleculeModelItemPrivate> d_ptr;
    QPixmap renderMolecule(const Molecule &molecule);
  protected:
    MoleculeModelItem();
    virtual Molecule* produceMolecule() const = 0;
    virtual bool performScaling() const;
  public:
    virtual ~MoleculeModelItem();
    QIcon icon();
    QString name();
    const Molecule* getMolecule();
    static MoleculeModelItem* fromXml(QByteArray xml);
    static MoleculeModelItem* fromMolecule(Molecule*);
  };
} // namespace Molsketch

#endif // MOLSKETCH_MOLECULEMODELITEM_H
