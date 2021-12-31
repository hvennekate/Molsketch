/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
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
 * This file is part of Molsketch and contains routines for loading and
 * saving files.
 *
 * @author Harm van Eersel
 * @since Hydrogen
 */

#include <QPrinter>

namespace Molsketch
{
class MolScene;
class Molecule;
class abstractXmlObject;

/**
 * Load and save routines
 *
 * @author Harm van Eersel
 */
/**
 * Prints the document on MolScene @p scene on QPrinter @p printer and
 * returns @c false if the print failed.
 */
  bool printFile(QPrinter &printer, MolScene * scene);
/**
 * Exports the current document on MolScene @p scene under @p fileName and
 * returns @c false if the export failed.
 */
  bool saveToSVG(const QString &fileName, MolScene * scene);
/**
 * Read file @p fileName in native fomat *.msk to @p scene.
 */
  void readMskFile(const QString &fileName, MolScene *scene);
/**
 * Read file @p fileName in native *.msk or *.msm format and @return all
 * molecules contained therein.
 */
  QList<Molecule*> moleculesFromFile(const QString &fileName);
/**
 * Write @p object to file @p fileName in native fomat *.msk.
 */
  bool writeMskFile(const QString &fileName, abstractXmlObject *scene);
/**
 * Exports the document on MolScene @p scene as a bitmap with @p fileName
 * and returns @c false if the export failed.
 */
  bool exportFile(const QString &fileName, MolScene * scene, const qreal &scaling);

}
