/***************************************************************************
 *   Copyright (C) 2014 by Hendrik Vennekate                               *
 *   HVennekate@gmx.de                                                     *
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

#ifndef OBABELIFACE_H
#define OBABELIFACE_H

#include <QPointF>
#include <vector>
#include <QString>

class QGraphicsScene ;

namespace Molsketch
{
  class Molecule ;
  class Atom ;
  class Bond ;
}

extern "C"
{

  namespace Molsketch
  {
/**
 * Get supported output file formats
 */
    QStringList outputFormats();
    typedef QStringList (*formatsFunctionPointer)() ;

/**
 * Get supported input file formats
 */
    QStringList inputFormats();
/**
 * Generate SMILES string from molecule
 */
    QString smiles(const Molecule*) ;
    typedef QString (*smilesFunctionPointer)(const Molecule*) ;
/**
 * Generate molecule from SMILES string
 */
    Molecule* fromSmiles(const QString&);
    typedef Molecule* (*fromSmilesFunctionPointer)(const QString&);
/**
 * Generate molecule from InChI string
 */
    Molecule* fromInChI(const QString&);
    typedef Molecule* (*fromInChIFunctionPointer)(const QString&);
/**
 * Check if InChI format is available
 */
    bool inChIAvailable();
    bool gen2dAvailable();
    typedef bool (*formatAvailablePointer)();
/**
 * Optimize coordinates
 */
    QVector<QPointF> optimizeCoordinates(const Molecule* molecule);
    typedef QVector<QPointF> (*optimizeCoordsPointer)(const Molecule*);

/**
 * Load and save routines
 *
 * @author Harm van Eersel (modified by H. Vennekate)
 */

/**
 * Loads file with @p fileName and returns it as pointer to a new Molecule
 * object.
 */
    Molecule* loadFile(const QString &fileName);
    typedef Molecule* (*loadFileFunctionPointer)(const QString&) ;
/**
 * Saves the current document under @p fileName and returns @c false if the
 * save failed.
 */
    bool saveFile(const QString &fileName, QGraphicsScene * scene, unsigned short int dim = 2);
    typedef bool (*saveFileFunctionPointer)(const QString&, QGraphicsScene*, unsigned short int) ;
/**
 * Get symmetry numbers for atoms
 */
    void getSymmetryClasses(const Molecule* molecule, std::vector<unsigned int>& symmetry_classes) ;
    typedef void (*getSymmetryClassesFunctionPointer)(const Molecule*, std::vector<unsigned int>&) ;
/**
 * Get chiral atoms for molecule @p molecule
 */
    QList<Atom*> chiralAtoms(const Molecule* molecule) ;
    typedef QList<Atom*> (*chiralAtomsFunctionPointer)(const Molecule*) ;
/**
 * Load Molecule from image (OSRA)
 */
    Molecule*  call_osra(QString fileName);
    typedef Molecule* (*callOsraFunctionPointer)(QString) ;
  } // namespace
} // extern "C"

#endif //OBABELIFACE_H
