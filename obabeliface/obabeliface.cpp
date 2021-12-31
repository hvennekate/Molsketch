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

#include <QFileInfo>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>

#include "obabeliface.h"
#include "molecule.h"
#include "bond.h"
#include "atom.h"
#include "molscene.h"

#include "scenesettings.h"
#include "settingsitem.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wdeprecated-copy" // OpenBabel 2 only

#include <openbabel/graphsym.h>
#include <openbabel/stereo/stereo.h>

#include <openbabel/mol.h>
#include <openbabel/data.h>
#include <openbabel/obconversion.h>
#include <openbabel/babelconfig.h>
#include <openbabel/op.h>
#include <openbabel/stereo/stereo.h>

#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
#include <openbabel/obiter.h>
#include <openbabel/bond.h>
#include <openbabel/elements.h>
#endif

#pragma GCC diagnostic pop

#if (OB_VERSION < OB_VERSION_CHECK(3, 0, 0))
OpenBabel::OBElementTable elementTable;
#endif

namespace Molsketch
{
  static const char INCHI_FORMAT[] = "inchi";

  QString number2symbol( int number )
  {
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
    return OpenBabel::OBElements::GetSymbol(number);
#else
    return elementTable.OBElementTable::GetSymbol(number);
#endif
  }

  int symbol2number( const QString &symbol )
  {
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
    return OpenBabel::OBElements::GetAtomicNum(symbol.toLatin1()) ;
#else
    return elementTable.GetAtomicNum(symbol.toLatin1()) ;
#endif
  }

  OpenBabel::OBMol toOBMolecule(const Molsketch::Molecule* originalMolecule, unsigned short int dim = 2)
  {
    // Create the output molecule
    OpenBabel::OBMol obmol ;
    obmol.SetDimension(dim) ;
    if (!originalMolecule) return obmol ;

    QHash<Atom*,OpenBabel::OBAtom*> hash;

    obmol.BeginModify();
    foreach (Atom* atom, originalMolecule->atoms()) {
      OpenBabel::OBAtom* obatom = obmol.NewAtom();
      obatom->SetVector(atom->scenePos().x()/40,atom->scenePos().y()/40,0);
      std::string element = atom->element().toStdString();
      obatom->SetAtomicNum(Molsketch::symbol2number(atom->element()));
      hash.insert(atom,obatom);
    }
    foreach (Bond* bond, originalMolecule->bonds()) {
      Atom* a1 = bond->beginAtom();
      Atom* a2 = bond->endAtom();

      int flags = 0 ;
      // Setting bondtype
      switch (bond->bondType()) {
      case Bond::Wedge:
        flags |= OB_WEDGE_BOND;
        break;
      case Bond::Hash:
        flags |= OB_HASH_BOND;
        break;
      default: break;
      }
      obmol.AddBond(hash[a1]->GetIdx(),
                      hash[a2]->GetIdx(),
                      bond->bondOrder(),
                      flags);
    }
    obmol.EndModify();
    return obmol;
  }

  QString smiles(const Molecule* mol)
  {
    OpenBabel::OBConversion conv ;
    if (!conv.SetOutFormat("can"))
      return "Output format 'can' not available." ;

    OpenBabel::OBMol obmol(toOBMolecule(mol)) ;
    return conv.WriteString(&obmol).c_str() ;
  }

  bool saveFile(const QString &fileName, QGraphicsScene* scene, unsigned short int dim) // TODO this should really take all the molecules instead of the entire scene!
  {
    using namespace OpenBabel;
    OBConversion conversion;

    if (!conversion.SetOutFormat(QFileInfo(fileName).suffix().toLatin1()))
    {
      qDebug() << "Error while saving #1";
      return false;
    }

    // Create the output molecule
    OBMol obmol;
    obmol.SetDimension(dim);

    // Add all molecules on the scene
    foreach(QGraphicsItem* item, scene->items()) // TODO remove this dependency on molscene
      if (auto molecule = dynamic_cast<Molecule*>(item))
        obmol += toOBMolecule(molecule, dim) ;
    // TODO this should really be done on a per-atom basis
    if (3 == dim // TODO also for 2D?
      && dynamic_cast<MolScene*>(scene)
      && dynamic_cast<MolScene*>(scene)->settings()->autoAddHydrogen()->get())
        obmol.AddHydrogens(); // TODO check if this works without begin/end modify

    // Checking if the file exists and making a backup
    if (QFile::exists(fileName))
    {
      QFile::remove(fileName + "~");
      QFile::copy(fileName,fileName + "~");
    }

    // Writing the final result to the file
    conversion.WriteFile(&obmol,fileName.toStdString());

    return true;
  }

  Molecule* fromOBMolecule(OpenBabel::OBMol& obmol) { // TODO convert title
    using namespace OpenBabel;
    // Create a new molecule
    Molecule* mol = new Molecule();
    mol->setPos(QPointF(0,0));

    qDebug() << "Number of atoms" <<obmol.NumAtoms();
    QHash<OBAtom*, Atom*> atomHash ;
    QHash<Atom*, int> charges;
    // Add atoms one-by-ons
    FOR_ATOMS_OF_MOL(obatom, obmol) {
      auto atom = new Atom(QPointF(obatom->x(), obatom->y()) * 40, Molsketch::number2symbol(obatom->GetAtomicNum()), true); // TODO this had to be done by MSK setting!
//      atom->setNumImplicitHydrogens(obatom->ImplicitHydrogenCount()); // TODO check if hydrogens can be transfered (reference: wiki search for diazomethane -- diazo-group appears to be wrong)
      charges[atom] = obatom->GetFormalCharge();
      atomHash[&(*obatom)] = atom;
      mol->addAtom(atom);
    }

    // Add bonds one-by-one
    /// Mind the numbering!
    FOR_BONDS_OF_MOL(obbond, obmol)
    {
      Bond* bond  = mol->addBond(atomHash[obbond->GetBeginAtom()],
                                 atomHash[obbond->GetEndAtom()],
                                 Bond::simpleTypeFromOrder(obbond->GetBondOrder()));
      // Set special bond types
      // TODO sometimes, when importing wikidata InChI molecules, this seems to lead to inverse bonds
      if (obbond->IsWedge())
        bond->setType( Bond::Wedge );
      if (obbond->IsHash())
        bond->setType( Bond::Hash );
    }

    // Set charges (has to be done _after_ setting up the bond network
    for (auto atom : charges.keys()) atom->setCharge(charges[atom]);

    return mol;
  }

  Molecule* loadFile(const QString &fileName)
  {
    // Creating and setting conversion classes
    using namespace OpenBabel;
    OBConversion conversion ;
    conversion.SetInFormat(conversion.FormatFromExt(fileName.toStdString())) ;
    conversion.AddOption("h", OBConversion::GENOPTIONS);
    OBMol obmol;

    if (!conversion.ReadFile(&obmol, fileName.toStdString()))
      return 0;

    return fromOBMolecule(obmol);
  }

  void getSymmetryClasses(const Molecule* molecule, std::vector<unsigned int>& symmetry_classes)
  {
    symmetry_classes.clear() ;
    if (!molecule) return ;
    OpenBabel::OBMol obmol(toOBMolecule(molecule)) ;
    OpenBabel::OBGraphSym graphsym(&obmol);
    graphsym.GetSymmetry(symmetry_classes);
  }

  QList<Atom*> chiralAtoms(const Molecule* molecule)
  {
    QList<Atom*> result ;
    if (!molecule) return result ;

    QList<Atom*> atoms(molecule->atoms()) ;
    OpenBabel::OBMol obmol(toOBMolecule(molecule)) ;
    // need to calculate symmetry first
    std::vector<unsigned int> symmetry_classes;
    OpenBabel::OBGraphSym graphsym(&obmol);
    graphsym.GetSymmetry(symmetry_classes);

    //std::vector<unsigned long> atomIds = FindTetrahedralAtoms(obmol, symmetry_classes);
    OpenBabel::OBStereoUnitSet units = FindStereogenicUnits(&obmol, symmetry_classes);

    for (unsigned int i = 0; i < units.size(); ++i) {
      if (units.at(i).type == OpenBabel::OBStereo::Tetrahedral) {
        OpenBabel::OBAtom *obatom = obmol.GetAtomById(units.at(i).id);
        result << atoms[obatom->GetIndex()] ;
      } else
      if (units.at(i).type == OpenBabel::OBStereo::CisTrans) {
        OpenBabel::OBBond *obbond = obmol.GetBondById(units.at(i).id);
        OpenBabel::OBAtom *obatom1 = obbond->GetBeginAtom();
        OpenBabel::OBAtom *obatom2 = obbond->GetEndAtom();
        result << atoms[obatom1->GetIndex()]
               << atoms[obatom2->GetIndex()] ;
      }
    }
    return result ;
  }

  Molecule* call_osra(QString fileName)
  {
    int n=0;
    QString tmpresult = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QDir::separator() + "osra";
    tmpresult += ".sdf";
    QString command;
    char *env = getenv("OSRA");
    if (env != NULL)
      command = env;
    else
      command = "osra";

    command += " -f sdf " + fileName + ">" + tmpresult;

    QStringList arguments;
    arguments << "-f";
    arguments << "sdf";
    arguments << fileName;
    arguments << ">";
    arguments << tmpresult;

    if (QProcess::execute(command, arguments))
      return 0;

    Molecule* mol = loadFile(tmpresult);
    if (mol) {
      qreal x_avg = 0, y_avg = 0;

      foreach(Atom* atom, mol->atoms()) {
        x_avg += atom->x();
        y_avg += atom->y();
        n++;
      }
      if (n > 0) {
        x_avg /= n;
        y_avg /= n;
        foreach(Atom* atom, mol->atoms())
          atom->setPos(atom->x() - x_avg, y_avg - atom->y());
      }
    }
    QFile::remove(tmpresult);
    return mol;
  }

  QStringList getFormats(const std::vector<std::string>& originalFormats) {
    QStringList formats;
    for (std::string pluginDescription : originalFormats) {
        QString fromOpenBabel(QString::fromStdString(pluginDescription));
        formats << fromOpenBabel.section(" -- ", 1) // This split is based on OBFormat::Display
                        + " (*." + fromOpenBabel.section(" -- ", 0, 0) + ")";
    }
    return formats;
  }

  QStringList outputFormats() {
    OpenBabel::OBConversion conversion;
    return getFormats(conversion.GetSupportedOutputFormat());
  }

  QStringList inputFormats() {
    OpenBabel::OBConversion conversion; // TODO find out why this needs to be instantiated
    return getFormats(conversion.GetSupportedInputFormat());
  }

  void SetWedgeAndHash(OpenBabel::OBMol& mol) {
      using namespace OpenBabel;
    // Remove any existing wedge and hash bonds
    FOR_BONDS_OF_MOL(b, &mol)  {
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
      b->SetWedge(false);
      b->SetHash(false);
#else
      b->UnsetWedge();
      b->UnsetHash();
#endif
    }

    std::map<OBBond*, enum OBStereo::BondDirection> updown;
    std::map<OBBond*, OBStereo::Ref> from;
    std::map<OBBond*, OBStereo::Ref>::const_iterator from_cit;
    TetStereoToWedgeHash(mol, updown, from);

    for(from_cit=from.begin();from_cit!=from.end();++from_cit) {
      OBBond* pbond = from_cit->first;
      if(updown[pbond]==OBStereo::UpBond)
        pbond->SetHash();
      else if(updown[pbond]==OBStereo::DownBond)
        pbond->SetWedge();
      else if(updown[pbond]==OBStereo::UnknownDir)
        pbond->SetWedgeOrHash();
    }
  }

  bool isInputFormatAvailable(OpenBabel::OBConversion conv, const char* format) {
    if (conv.SetInFormat(format)) return true;
    qCritical("Could not find format: %s", format);
    qInfo() << ("Available formats: " + outputFormats().join(", "));
    return false;
  }

  void generate2dCoords(OpenBabel::OBMol& obmol)
  {
    OpenBabel::OBOp* gen2D = OpenBabel::OBOp::FindType("gen2D");
    if (!gen2D || !gen2D->Do(&obmol))
      qCritical("Could not find gen2D for coordinate generation.");
  }

  Molecule *fromString(const QString &input, const char* format) {
    OpenBabel::OBConversion conv ;
    qDebug() << "setting input format" << format;
    if (!conv.SetInFormat(format)) {
      qCritical() << "Could not find format:" << format;
      qInfo() << "Available formats:" << outputFormats().join(", ");
      return 0;
    }
    conv.AddOption("h", OpenBabel::OBConversion::GENOPTIONS);

    OpenBabel::OBMol obmol;
    qDebug() << "reading molecule" << input;
    if (!conv.ReadString(&obmol, input.toStdString())) {
      qCritical() << "Could not convert InChI:" << input; // TODO do we need error handling if false?
      return nullptr;
    }
    qDebug() << "Error messages:" << QString::fromStdString(OpenBabel::OBMessageHandler().GetMessageSummary());

    generate2dCoords(obmol);
    SetWedgeAndHash(obmol);
    return fromOBMolecule(obmol);
  }

  Molecule *fromSmiles(const QString &input) {
    return fromString(input, "can");
  }

  Molecule *fromInChI(const QString &input) {
    return fromString(input.startsWith("InChI=") ? input : "InChI=" + input, INCHI_FORMAT);
  }

  bool inChIAvailable() {
    return isInputFormatAvailable(OpenBabel::OBConversion(), INCHI_FORMAT);
  }

  bool gen2dAvailable() {
    return OpenBabel::OBOp::FindType("gen2D");
  }

  QVector<QPointF> optimizeCoordinates(const Molecule *molecule) {
    OpenBabel::OBMol obmol(toOBMolecule(molecule));
    generate2dCoords(obmol);
    QPolygonF optimizedCoordinates = fromOBMolecule(obmol)->coordinates();
    optimizedCoordinates.translate(
          molecule->coordinates().boundingRect().center() -
          optimizedCoordinates.boundingRect().center());
    return optimizedCoordinates;
  }

} // namespace
