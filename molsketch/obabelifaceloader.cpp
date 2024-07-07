/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate                               *
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
#include "obabelifaceloader.h"
#include "obabeliface.h"

#include <QDebug>
#include <QLibrary>
#include <molecule.h>
#include <QMessageBox>

#define ENCAPSULE_OB_CALL(CALL) try { CALL } catch (...) { \
  QMessageBox::critical(nullptr, "Error", "OpenBabel/obabeliface threw an exception. Please report to the author."); }

static const char BABEL_LIBDIR_VARIABLE[] = "BABEL_LIBDIR"; // TODO take from OpenBabel directly

class OBabelIfaceLoaderPrivate {
  Q_DISABLE_COPY (OBabelIfaceLoaderPrivate)
  public:
  OBabelIfaceLoaderPrivate (OBabelIfaceLoader* parent) : parent (parent) {}
  OBabelIfaceLoader* parent;
  QLibrary openBabelInterface;
  QString openBabelFormatsPath;
  Molsketch::loadFileFunctionPointer load;
  Molsketch::saveFileFunctionPointer save;
  Molsketch::formatsFunctionPointer inputFormats;
  Molsketch::formatsFunctionPointer outputFormats;
  Molsketch::fromInChIFunctionPointer fromInChI;
  Molsketch::optimizeCoordsPointer optimizeCoordinates;
  Molsketch::formatAvailablePointer inChIAvailable;
  Molsketch::formatAvailablePointer gen2dAvailable;
  void loadFunctions () {
    load = (Molsketch::loadFileFunctionPointer)openBabelInterface.resolve (
      "loadFile"); // TODO put into obabeliface.h -- question: what happens when lib is dynamically loaded (with its constants etc.)?
    save = (Molsketch::saveFileFunctionPointer)openBabelInterface.resolve ("saveFile");
    inputFormats = (Molsketch::formatsFunctionPointer)openBabelInterface.resolve ("inputFormats");
    outputFormats = (Molsketch::formatsFunctionPointer)openBabelInterface.resolve ("outputFormats");
    fromInChI = (Molsketch::fromInChIFunctionPointer)openBabelInterface.resolve ("fromInChI");
    optimizeCoordinates = (Molsketch::optimizeCoordsPointer)openBabelInterface.resolve("optimizeCoordinates");
    inChIAvailable = (Molsketch::formatAvailablePointer)openBabelInterface.resolve ("inChIAvailable");
    gen2dAvailable = (Molsketch::formatAvailablePointer)openBabelInterface.resolve("gen2dAvailable");

    qDebug() << "Loaded OpenBabel functions. Available Functions:"
             << "load:" << load
             << "save:" << save
             << "inputFormats:" << inputFormats
             << "outputFormats:" << outputFormats
             << "fromInChI:" << fromInChI
             << "gen2dAvailable:" << gen2dAvailable
             << "optimizeCoordinates:" << optimizeCoordinates
             << "inChIAvailable:" << inChIAvailable
             << "Error:" << openBabelInterface.errorString();
  }
  void unloadFunctions () { // TODO check if this is really necessary
  }
  void emitSignals () {
    bool openBabelAvailable = openBabelInterface.isLoaded ();
    bool isInchiAvailable = inChIAvailable && inChIAvailable ();
    bool isGen2dAvailable = gen2dAvailable && gen2dAvailable ();
    qDebug() << "OpenBabel available: " << QString::number (openBabelAvailable)
             << "Library location:" << openBabelInterface.fileName();
    qDebug() << "InChI available: " << QString::number (isInchiAvailable)
             << "gen2d available: " << QString::number (isGen2dAvailable);
    emit parent->obabelIfaceAvailable (openBabelAvailable);
    emit parent->inchiAvailable (isInchiAvailable && isGen2dAvailable);
    emit parent->optimizeAvailable(isGen2dAvailable);
    emit parent->obabelIfaceFileNameChanged(openBabelInterface.fileName());
  }
};

OBabelIfaceLoader::OBabelIfaceLoader (QObject* parent) : QObject (parent), d_ptr (new OBabelIfaceLoaderPrivate (this)) {}

OBabelIfaceLoader::~OBabelIfaceLoader () { delete d_ptr; }

template<class Output, class Input, typename Transformation>
Output convertContainer(Input inputContainer, Transformation transformation = [] (auto item) { return item; }) {
  Output result;
  std::transform(inputContainer.cbegin(), inputContainer.cend(), result.begin(), transformation);
  return result;
}

QStringList toQStringList(const std::vector<std::string>& vectorOfStrings) {
  return convertContainer<QStringList>(vectorOfStrings, &QString::fromStdString);
}

QStringList OBabelIfaceLoader::inputFormats () {
  Q_D (OBabelIfaceLoader);
  ENCAPSULE_OB_CALL(if (d->inputFormats) return toQStringList(d->inputFormats());)
  qWarning ("No OpenBabel formats available for input");
  return QStringList ();
}

QStringList OBabelIfaceLoader::outputFormats () {
  Q_D (OBabelIfaceLoader);
  ENCAPSULE_OB_CALL(if (d->outputFormats) return toQStringList(d->outputFormats());)
  qWarning ("No OpenBabel formats available for output");
  return QStringList ();
}

Molsketch::Molecule* OBabelIfaceLoader::loadFile (std::istream *input, const std::string &filename, qreal scaling) {
  Q_D (OBabelIfaceLoader);
  ENCAPSULE_OB_CALL(if (d->load) return Molsketch::Molecule::fromCoreMolecule(d->load (input, filename), scaling);)
  qWarning ("No OpenBabel support available for loading file");
  return nullptr;
}

bool OBabelIfaceLoader::saveFile (std::ostream *output, const std::string &filename, const QList<Molsketch::Molecule *> &originalMolecules, bool use3d, bool addHydrogens, qreal scaling) {
  auto nonNullMolecules = QList<Molsketch::Molecule*>();
  std::copy_if(originalMolecules.cbegin(), originalMolecules.cend(), nonNullMolecules.begin(), [] (auto *input) { return input != nullptr; });
  auto molecules = convertContainer<std::vector<Molsketch::Core::Molecule>>(nonNullMolecules, [&] (Molsketch::Molecule* original) { return original->toCoreMolecule(scaling); });
  Q_D (OBabelIfaceLoader);
  ENCAPSULE_OB_CALL(if (d->save) return d->save (output, filename, molecules, use3d ? 3 : 2, addHydrogens);)
  qWarning ("No support for saving OpenBabel available");
  return false;
}

Molsketch::Molecule *OBabelIfaceLoader::convertInChI(const QString &InChI) {
  // TODO this should also have a scaling parameter!
  Q_D(OBabelIfaceLoader);
  ENCAPSULE_OB_CALL(if (d->fromInChI) return Molsketch::Molecule::fromCoreMolecule(d->fromInChI(InChI.toStdString()));)
  qWarning("No support for converting InChI available");
  return nullptr;
}

QVector<QPointF> OBabelIfaceLoader::optimizeCoordinates(const Molsketch::Molecule *molecule) {
  Q_D(OBabelIfaceLoader);
  ENCAPSULE_OB_CALL(if (d->optimizeCoordinates)
                    return convertContainer<QVector<QPointF>>
                    (d->optimizeCoordinates(molecule->toCoreMolecule()), [] (auto item) { return QPointF(item.getX(), item.getY()); }); )
  qWarning("No support for optimizing coordinates using OpenBabel available");
  return molecule->coordinates();
}

void OBabelIfaceLoader::reloadObabelIface (const QString& path) {
  Q_D (OBabelIfaceLoader);
  d->openBabelInterface.unload ();
  d->openBabelInterface.setFileName (path);
  d->openBabelInterface.setLoadHints(QLibrary::ExportExternalSymbolsHint);
  d->loadFunctions ();
  d->emitSignals ();
}

void OBabelIfaceLoader::setObabelFormats (const QString& folder) {
  Q_D (OBabelIfaceLoader);
  d->openBabelFormatsPath = folder;
  qputenv (BABEL_LIBDIR_VARIABLE, folder.toUtf8 ()); // TODO: Latin1?
  d->emitSignals ();
}
