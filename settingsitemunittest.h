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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "qrealsignalcounter.h"

#include <cxxtest/TestSuite.h>
#include <settingsfacade.h>
#include <settingsitem.h>

using namespace Molsketch;

const QString KEY("TestKey");
const qreal DOUBLE_VALUE = 3.75;
const QVariant VARIANT(DOUBLE_VALUE);
const QString VALUE_AS_STRING("3.75");
const char* DESCRIPTION = "Test description";
const qreal ALTERNATE_VALUE = 4.5;
const QString ALTERNATE_STRING("4.5");
const QVariant ALTERNATE_VARIANT(4.5);


class SettingsItemUnitTest : public CxxTest::TestSuite {
  SettingsFacade *facade;
  QRealSignalCounter *qrealSignalCounter;

  template<typename T>
  void performSetValueTest(const T& newValue) {
    DoubleSettingsItem doubleSettingsItem(DESCRIPTION, KEY, facade);
    QObject::connect(&doubleSettingsItem, SIGNAL(updated(qreal)), qrealSignalCounter, SLOT(record(qreal)));

    doubleSettingsItem.set(newValue);

    TS_ASSERT_EQUALS(facade->value(KEY), ALTERNATE_VARIANT);
    qrealSignalCounter->assertPayloads({ALTERNATE_VALUE});
  }

public:
  void setUp() {
    facade = SettingsFacade::transientSettings();
    facade->setValue(KEY, DOUBLE_VALUE);
    qrealSignalCounter = new QRealSignalCounter(facade);
  }

  void tearDown() {
    delete facade;
  }

  void testObtainingDoubleValue() {
    TS_ASSERT_EQUALS(DoubleSettingsItem(DESCRIPTION, KEY, facade).get(), DOUBLE_VALUE);
    TS_ASSERT_EQUALS(DoubleSettingsItem(DESCRIPTION, KEY, facade).serialize(), VALUE_AS_STRING);
    TS_ASSERT_EQUALS(DoubleSettingsItem(DESCRIPTION, KEY, facade).getVariant(), VARIANT);
  }

  void testSettingValue() {
    performSetValueTest(ALTERNATE_VALUE);
  }

  void testSettingString() {
    performSetValueTest(ALTERNATE_STRING);
  }

  void testSettingVariant() {
    performSetValueTest(ALTERNATE_VARIANT);
  }

  void testProducingWidget() {

  }
};
