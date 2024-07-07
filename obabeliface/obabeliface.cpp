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

#include "obabeliface.h"
#include "coremolecule.h"
#include "corebond.h"
#include "coreatom.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated-copy" // OpenBabel 2 only

#include <openbabel/graphsym.h>

#include <openbabel/mol.h>
#include <openbabel/data.h>
#include <openbabel/obconversion.h>
#include <openbabel/babelconfig.h>
#include <openbabel/op.h>
#include <openbabel/stereo/stereo.h>
#include <openbabel/stereo/tetrahedral.h>

#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
#include <openbabel/obiter.h>
#include <openbabel/bond.h>
#include <openbabel/elements.h>
#endif

#include <iostream>
#include <numeric>

#pragma GCC diagnostic pop

#if (OB_VERSION < OB_VERSION_CHECK(3, 0, 0))
OpenBabel::OBElementTable elementTable;
#endif

namespace Molsketch
{
  using Core::Molecule;
  using Core::Atom;
  using Core::Bond;
  static const char INCHI_FORMAT[] = "inchi";

  std::string number2symbol( int number )
  {
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
    return OpenBabel::OBElements::GetSymbol(number);
#else
    return elementTable.OBElementTable::GetSymbol(number);
#endif
  }

  int symbol2number( const std::string &symbol )
  {
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
    return OpenBabel::OBElements::GetAtomicNum(symbol.data()) ;
#else
    return elementTable.GetAtomicNum(symbol.data()) ;
#endif
  }

  OpenBabel::OBMol toOBMolecule(const Molecule &originalMolecule, unsigned short int dim = 2)
  {
    // Create the output molecule
    OpenBabel::OBMol obmol ;
    obmol.SetDimension(dim) ;

    std::vector<OpenBabel::OBAtom*> newAtoms;

    obmol.BeginModify();
    for (auto atom : originalMolecule.atoms()) {
      newAtoms.push_back(obmol.NewAtom());
      auto newAtom = newAtoms.back();
      newAtom->SetVector(atom.position().getX(), atom.position().getY(), 0);
      newAtom->SetAtomicNum(Molsketch::symbol2number(atom.element()));
      newAtom->SetFormalCharge(atom.charge()); // TODO does this have to be done after the bonds?
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
      newAtom->SetImplicitHCount(atom.hAtoms());
#endif
    }

    for (auto bond : originalMolecule.bonds()) {
      if (bond.order() < 1) continue;

      OpenBabel::OBBond newBond;
      newBond.SetBondOrder(bond.order());
      newBond.SetBegin(newAtoms.at(bond.start()));
      newBond.SetEnd(newAtoms.at(bond.end()));

      if (Bond::Wedge == bond.type()) newBond.SetWedge();
      if (Bond::Hash == bond.type()) newBond.SetHash();
      if (Bond::WedgeOrHash == bond.type()) newBond.SetWedgeOrHash();
      obmol.AddBond(newBond);
    }
    obmol.EndModify();

#if (OB_VERSION < OB_VERSION_CHECK(3, 0, 0))
    for (int i = 0 ; i < newAtoms.size() ; ++i) {
      auto newAtom = newAtoms.at(i);
      auto atom = originalMolecule.atoms().at(i);
      std::cerr << "H atoms" << atom.hAtoms()
                << " implicit count: " << newAtom->ImplicitHydrogenCount()
                << " exlicit count: " << newAtom->ExplicitHydrogenCount()
                << " valence: " << newAtom->GetImplicitValence()
                << endl;
      newAtom->SetSpinMultiplicity(qAbs(2*(newAtom->ImplicitHydrogenCount() - atom.hAtoms())));
      std::cerr << "after set H atoms " << atom.hAtoms()
                << " implicit count: " << newAtom->ImplicitHydrogenCount()
                << " exlicit count: " << newAtom->ExplicitHydrogenCount()
                << " valence: " << newAtom->GetImplicitValence()
                << " multiplicity: " << newAtom->GetSpinMultiplicity()
                << endl;
    }
#endif

    return obmol;
  }

  bool saveFile(std::ostream *output, const std::string &filename, const std::vector<Molecule> &molecules, unsigned short int dim, bool addHydrogens)
  {
    using namespace OpenBabel;
    OBConversion conversion;

    if (!conversion.SetOutFormat(conversion.FormatFromExt(filename))) {
      std::cerr << "Error while saving: " << filename << std::endl;
      return false;
    }

    OBMol obmol;
    obmol.SetDimension(dim);
    for (auto molecule : molecules)
      obmol += toOBMolecule(molecule, dim);
    if (addHydrogens) obmol.AddHydrogens(); // TODO check if this works without begin/end modify
    conversion.Write(&obmol, output);
    return true;
  }

  Molecule fromOBMolecule(OpenBabel::OBMol& obmol) {
    using namespace OpenBabel;

    std::vector<Atom> atoms;
    std::map<OBAtom*, unsigned> atomNumbers;
    int i = 0;
    FOR_ATOMS_OF_MOL(obatom, obmol) {
      atoms.push_back(Atom(Molsketch::number2symbol(obatom->GetAtomicNum()),
                    Core::Position(obatom->x(), obatom->y()),
#if (OB_VERSION >= OB_VERSION_CHECK(3, 0, 0))
                    obatom->GetImplicitHCount(),
#else
                    obatom->ImplicitHydrogenCount(),
#endif
                    obatom->GetFormalCharge()));
      atomNumbers[&(*obatom)] = i++;
    }

    std::vector<Bond> bonds;
    FOR_BONDS_OF_MOL(obbond, obmol)
    {
      Bond::Type type;
      if (obbond->IsWedge()) type = Bond::Wedge;
      else if (obbond->IsHash()) type = Bond::Hash;
      else type = Bond::fromOrder(obbond->GetBondOrder());

      bonds.push_back(Bond(atomNumbers[obbond->GetBeginAtom()],
          atomNumbers[obbond->GetEndAtom()],
          type));
    }

    return Molecule(atoms, bonds, obmol.GetTitle());
  }

  // TODO should be const, but OpenBabel iterator methods do not support const
  bool hasCoordinates(OpenBabel::OBMol &molecule) {
    FOR_ATOMS_OF_MOL(obatom, molecule) {
      if (obatom->GetVector() != OpenBabel::VZero)
        return true;
    }
    return false;
  }

  void generate2dCoords(OpenBabel::OBMol& obmol)
  {
    OpenBabel::OBOp* gen2D = OpenBabel::OBOp::FindType("gen2D");
    if (!gen2D || !gen2D->Do(&obmol))
      std::cerr << "Could not find gen2D for coordinate generation." << std::endl;
  }

  void setWedgeAndHash(OpenBabel::OBMol& mol) {
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

    // TODO reverse bonds where their direction is not "from stereo center"

    std::map<OBBond*, enum OBStereo::BondDirection> updown;
    std::map<OBBond*, OBStereo::Ref> from;
    std::map<OBBond*, OBStereo::Ref>::const_iterator from_cit;
    TetStereoToWedgeHash(mol, updown, from);

    for(from_cit = from.begin(); from_cit != from.end(); ++from_cit) {
      OBBond* pbond = from_cit->first;
      if(updown[pbond]==OBStereo::UpBond)
        pbond->SetHash();
      else if(updown[pbond]==OBStereo::DownBond)
        pbond->SetWedge();
      else if(updown[pbond]==OBStereo::UnknownDir)
        pbond->SetWedgeOrHash();
    }
  }

  Molecule loadFile(std::istream *input, const std::string &filename)
  {
    // Creating and setting conversion classes
    using namespace OpenBabel;
    OBConversion conversion ;
    conversion.SetInFormat(conversion.FormatFromExt(filename)) ;
    conversion.AddOption("h", OBConversion::GENOPTIONS); // add hydrogens
    OBMol obmol;

    if (!conversion.Read(&obmol, input)) return Molecule({}, {});

    if (!hasCoordinates(obmol)) {
      generate2dCoords(obmol);
      setWedgeAndHash(obmol); // TODO check if this is only needed if no original coords were present or generally for all formats
    }

    return fromOBMolecule(obmol);
  }

  std::vector<std::string> getFormats(const std::vector<std::string>& originalFormats) {
    std::vector<std::string> formats;
    for (std::string pluginDescription : originalFormats) {
        auto splitPos = pluginDescription.find(" -- "); // This split is based on OBFormat::Display
        formats.push_back(pluginDescription.substr(splitPos + 4)
                        + " (*." + pluginDescription.substr(0, splitPos) + ")");
    }
    return formats;
  }

  std::vector<std::string> outputFormats() {
    OpenBabel::OBConversion conversion;
    return getFormats(conversion.GetSupportedOutputFormat());
  }

  std::vector<std::string> inputFormats() {
    OpenBabel::OBConversion conversion;
    return getFormats(conversion.GetSupportedInputFormat());
  }

  void printAvailableOutputFormats() {
    auto outputs = outputFormats();
    std::cout << "Available formats: ";
    for (auto it = outputs.cbegin(); it != outputs.cend(); ++it) {
      std::cout << *it;
      if (it + 1 < outputs.cend()) std::cout << ", ";
    }
  }

  bool isInputFormatAvailable(OpenBabel::OBConversion conv, const char* format) {
    if (conv.SetInFormat(format)) return true;
    std::cerr << "Could not find format: " << format << std::endl;
    printAvailableOutputFormats();
    return false;
  }

  Molecule fromString(const std::string &input, const char* format) {
    OpenBabel::OBConversion conv ;
    std::cout << "setting input format " << format << std::endl;
    if (!conv.SetInFormat(format)) {
      std::cerr << "Could not find format: " << format << std::endl;
      printAvailableOutputFormats();
      return Molecule({}, {});
    }
    conv.AddOption("h", OpenBabel::OBConversion::GENOPTIONS);

    OpenBabel::OBMol obmol;
    std::cout << "reading molecule " << input << std::endl;
    if (!conv.ReadString(&obmol, input)) {
      std::cerr << "Could not convert InChI:" << input << std::endl; // TODO do we need error handling if false?
      return Molecule({}, {});
    }
    std::cout << "Error messages: " << OpenBabel::OBMessageHandler().GetMessageSummary() << std::endl;

    generate2dCoords(obmol);
    setWedgeAndHash(obmol);
    return fromOBMolecule(obmol);
  }

  Molsketch::Core::Molecule fromInChI(const std::string &input) {
    return fromString(input.starts_with("InChI=") ? input : "InChI=" + input, INCHI_FORMAT);
  }

  bool inChIAvailable() {
    return isInputFormatAvailable(OpenBabel::OBConversion(), INCHI_FORMAT);
  }

  bool gen2dAvailable() {
    return OpenBabel::OBOp::FindType("gen2D");
  }

  std::vector<Core::Position> optimizeCoordinates(const Molecule &molecule) {
    OpenBabel::OBMol obmol(toOBMolecule(molecule));
    generate2dCoords(obmol);
    auto optimizedMolecule = fromOBMolecule(obmol);
    auto shift = molecule.center() - optimizedMolecule.center();
    auto optimizedCoordinates = optimizedMolecule.coordinates();
    for (auto position : optimizedCoordinates)
        position += shift;
    return optimizedCoordinates;
  }

} // namespace
