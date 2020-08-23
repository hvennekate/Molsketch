/***************************************************************************
 *   Copyright (C) 2020 by Hendrik Vennekate                               *
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
#include <QCoreApplication>
#include <QMutex>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <atomicmassloader.h>
#include "utilities.h"

using namespace Molsketch;

class WikiDataIsotopetest : public CxxTest::TestSuite {
public:
  void testLoadElements() {
    bool done = false; // TODO facilities for testing async processes/signals

    AtomicMassLoader massLoader;
    QObject::connect(&massLoader, &AtomicMassLoader::elementMassesUpdated, [&] { done = true; });
    massLoader.updateElementMasses();

    while (!done) QCoreApplication::processEvents();

    // TODO iterable assertions
    auto massMap = massLoader.getAtomicMasses();
    auto it = massMap.cbegin();
    while (it != massMap.cend()) {
      QSM_ASSERT_NOT_EQUALS("Null mass found for element " + it.key(), it.value(), 0.);
      ++it;
    }

    QS_ASSERT_EQUALS(massMap.size(), 89);
    TS_ASSERT_DELTA(massMap["Na"], 22.9898, 1e-4);
  }

  void testLoadIsotopes() {
    bool done = false;

    QTimer timeLimit;
    timeLimit.callOnTimeout([&] { TSM_ASSERT("Response not received within 10 s.", false); done = true; });
    timeLimit.start(10000);

    AtomicMassLoader massLoader;
    QObject::connect(&massLoader, &AtomicMassLoader::isotopeMassesUpdated, [&] { done = true; });
    QObject::connect(&massLoader, &AtomicMassLoader::isotopeMassesUpdated, &timeLimit, &QTimer::stop);
    massLoader.updateIsotopeMasses();

    while (!done) QCoreApplication::processEvents();

    auto isotopeMap = massLoader.getIsotopeMasses();
    auto it = isotopeMap.cbegin();
    while (it != isotopeMap.cend()) {
      QSM_ASSERT_NOT_EQUALS("Null mass found for isotope " + it.key().first + it.key().second, it.value(), 0.);
      ++it;
    }
    QS_ASSERT_EQUALS(isotopeMap.size(), 3434);
    TS_ASSERT_DELTA(isotopeMap[std::make_pair(QString{"U"}, 235)], 235.0440, 1e-4);
  }
};
