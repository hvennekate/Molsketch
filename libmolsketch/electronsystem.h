/***************************************************************************
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
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
#ifndef MSK_ELECTRONSYSTEM_H
#define MSK_ELECTRONSYSTEM_H

#include <QList>

namespace Molsketch {

  class Atom;
  class Molecule;

  /**
   * Base class for electron systems.
   *
   * The ElectronSystem class and it's subclasses are an alternative way to
   * store how electrons are distributed in a molecule. It overcomes many
   * problems associated with connection tables and can be used to represent
   * a wide variety molecules.
   */
  class ElectronSystem
  {
    public:
      enum Type {
        SigmaType,
        PiType,
        CoordType,
        DeficientSigmaType,
      };

      explicit ElectronSystem(const QList<Atom*>& atoms = QList<Atom*>(), int numElectrons = 0);
      virtual ~ElectronSystem() {}

      /**
       * Get the type for this electron system
       */
      virtual Type type() const = 0;


      int numAtoms() const;
      QList<Atom*> atoms() const; // TODO return set
      void setAtoms(const QList<Atom*> &atoms);
      void setAtoms(const QPair<Atom*, Atom*> &atoms);

      int numElectrons() const;
      void setNumElectrons(int numElectrons);

    protected:
      Molecule     *m_molecule;
      QList<Atom*>  m_atoms;
      int           m_numElectrons;
  };


  class SigmaElectrons : public ElectronSystem
  {
    public:
      enum Geometry {
        TwoCenter, //!< regular 2 electron, 2 center sigma system
        Bridged, //!< bridged sigma electron system (e.g. B-H-B in B5H11, transition states, ...)
        Closed //!< closed sigma electron system (e.g. BBB in B5H11)
      };

      SigmaElectrons(Geometry geometry = TwoCenter) : ElectronSystem(), m_geometry(geometry) {}
      virtual ~SigmaElectrons() {}

      Type type() const override { return SigmaType; }

      Geometry getGeometry() const { return m_geometry; }
      void setGeometry(Geometry geometry) { m_geometry = geometry; }

    protected:
      Geometry m_geometry;
  };

  class PiElectrons : public ElectronSystem
  {
    public:
      PiElectrons() : ElectronSystem() {}
      virtual ~PiElectrons() {}
      PiElectrons(QList<Atom*> atoms, int numElectrons);
      PiElectrons(const QPair<Atom*, Atom*>& atoms, int numElectrons);

      Type type() const override { return PiType; }
  };

  class CoordElectrons : public ElectronSystem
  {
    public:
      CoordElectrons() : ElectronSystem() {}
      virtual ~CoordElectrons() {}

      Type type() const override { return CoordType; }
  };



}

#endif
