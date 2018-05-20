/***************************************************************************
 *   Copyright (C) 2016 by Hendrik Vennekate                               *
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

#include <cxxtest/TestSuite.h>

#include <molecule.h>
#include <QSet>

using namespace Molsketch;

class MoleculeTest : public CxxTest::TestSuite
{
  typedef QPair<int,int> c; // connection
  typedef QVector<c> cv; //connectivity vector
  struct MoleculeToplogy {
    QString testName;
    int atomCount;
    int parts;
    cv connections;
    MoleculeToplogy(const QString& testName, int atomCount, int parts, cv connections)
      : testName(testName), atomCount(atomCount), parts(parts), connections(connections) {}

    Molecule* molecule() {
      QList<Atom*> atoms = this->atoms();
      QSet<Bond*> bonds = this->bonds(atoms);
      return new Molecule(atoms.toSet(), bonds);
    }

    QList<Atom*> atoms()
    {
      QList<Atom*> atoms;
      for (int i = 0 ; i < atomCount ; ++i)
        atoms << new Atom(QPointF((qreal) qrand()/RAND_MAX,(qreal) qrand()/RAND_MAX), "C" + QString::number(i));
      return atoms;
    }

    QSet<Bond*> bonds(QList<Atom*> atoms)
    {
      QSet<Bond*> bonds;
      for(c connection : connections)
        bonds << new Bond(atoms[connection.first], atoms[connection.second]);
      return bonds;
    }

    void runAssertions() {
      Molecule *molecule = this->molecule();
      QList<Molecule*> parts = molecule->split();

      TSM_ASSERT_EQUALS(testName + " can split", molecule->canSplit(), this->parts > 1);
      TSM_ASSERT_EQUALS(testName + " number of parts", parts.size(), this->parts);
      for (Molecule* part : parts) {
        TSM_ASSERT_EQUALS(testName + " new atoms distinct", molecule->atoms().toSet() & part->atoms().toSet(), QSet<Atom*>());
        TSM_ASSERT_EQUALS(testName + " new bonds distinct", molecule->bonds().toSet() & part->bonds().toSet(), QSet<Bond*>());
      }
    }
  };

public:
  void testSplitting() {
    QList<MoleculeToplogy> testCases;
    testCases << MoleculeToplogy("Cyclopropane", 3, 1, cv() << c(0,1) << c(1,2) << c(2,0))
              << MoleculeToplogy("atom and bond", 3, 2, cv() << c(2,0))
              << MoleculeToplogy("three atoms", 3, 3, cv())
              << MoleculeToplogy("four atoms and bond and atom", 7, 3, cv() << c(1,0) << c(2,0) << c(1,3) << c(4,5))
              << MoleculeToplogy("ring and bond", 5, 2, cv() << c(1,0) << c(2,0) << c(1,2) << c(3,4))
              << MoleculeToplogy("null case", 0, 0, cv());

    for(auto testCase : testCases) testCase.runAssertions();
  }
};
