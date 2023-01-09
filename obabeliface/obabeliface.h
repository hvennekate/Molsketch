/***************************************************************************
 *   Copyright (C) 2014 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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

#ifdef Q_OS_WIN
#define EXPORT_PREFIX __declspec(dllexport)
#else
#define EXPORT_PREFIX
#endif

namespace Molsketch::Core
{
  class Molecule ;
  class Atom ;
  class Bond ;
}

extern "C"
{

  namespace Molsketch
  {
    EXPORT_PREFIX QStringList outputFormats();
    typedef QStringList (*formatsFunctionPointer)() ;
    const char OUTPUT_FORMATS[] = "outputFormats";

    EXPORT_PREFIX QStringList inputFormats();
    const char INPUT_FORMATS[] = "inputFormats";

    EXPORT_PREFIX Core::Molecule fromInChI(const QString&); // TODO add name
    const char FROM_INCHI[] = "fromInChI";
    typedef Core::Molecule (*fromInChIFunctionPointer)(const QString&);

    EXPORT_PREFIX bool inChIAvailable();
    const char INCHI_AVAILABLE[] = "inChIAvailable";
    typedef bool (*formatAvailablePointer)();

    EXPORT_PREFIX bool gen2dAvailable();
    const char GEN2D_AVAILABLE[] = "gen2dAvailable";

    EXPORT_PREFIX QVector<QPointF> optimizeCoordinates(const Core::Molecule &molecule);
    const char OPTIMIZE_COORDS[] = "optimizeCoordinates";
    typedef QVector<QPointF> (*optimizeCoordsPointer)(const Core::Molecule &);

    EXPORT_PREFIX Core::Molecule loadFile(std::istream *input, const std::string &filename);
    const char LOAD_FILE[] = "loadFile";
    typedef Core::Molecule (*loadFileFunctionPointer)(std::istream *, const std::string &) ;

    EXPORT_PREFIX bool saveFile(std::ostream *output, const std::string &filename, const QList<Core::Molecule> &molecules, unsigned short int dim = 2, bool addHydrogens = false);
    const char SAVE_FILE[] = "saveFile";
    typedef bool (*saveFileFunctionPointer)(std::ostream *, const std::string &, const QList<Core::Molecule> &, unsigned short int, bool) ;

    EXPORT_PREFIX Core::Molecule call_osra(QString fileName);
    const char CALL_OSRA[] = "call_osra";
    typedef Core::Molecule (*callOsraFunctionPointer)(QString) ;
  } // namespace
} // extern "C"

#endif //OBABELIFACE_H
