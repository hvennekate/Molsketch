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

#include "signalcounter.h"

#include <cxxtest/TestSuite.h>
#include <settingsfacade.h>
#include <settingsitem.h>
#include <stringify.h>

using namespace Molsketch;

const QString DOUBLE_KEY("TestDoubleKey");
const qreal DOUBLE_VALUE = 3.75;
const QString DOUBLE_AS_STRING("3.75");
const qreal ALTERNATE_DOUBLE = 4.5;
const QString ALTERNATE_DOUBLE_AS_STRING("4.5");

const QString BOOL_KEY("TestBoolKey");
const bool BOOL_VALUE = true;
const QString BOOL_AS_STRING("true");
const bool ALTERNATE_BOOL = false;
const QString ALTERNATE_BOOL_AS_STRING("false");

const QString COLOR_KEY("TestColorKey");
const QColor COLOR_VALUE(Qt::red);
const QString COLOR_AS_STRING("Af////8AAAAAAAA=");
const QColor ALTERNATE_COLOR(Qt::blue);
const QString ALTERNATE_COLOR_AS_STRING("Af//AAAAAP//AAA=");

const QString FONT_KEY("TestFontKey");
const QFont FONT_VALUE("Times", 10, QFont::Bold);
const QString FONT_AS_STRING(stringify(FONT_VALUE));
const QFont ALTERNATE_FONT("Helvetica", 15, QFont::Cursive);
const QString ALTERNATE_FONT_AS_STRING(stringify(ALTERNATE_FONT));

const QString STRING_LIST_KEY("TestStringListKey");
const QStringList STRING_LIST_VALUE{"testString1", "testString2"};
const QString STRING_LIST_AS_STRING("AAAAAgAAABYAdABlAHMAdABTAHQAcgBpAG4AZwAxAAAAFgB0AGUAcwB0AFMAdAByAGkAbgBnADI=");
const QStringList ALTERNATE_STRING_LIST{"alternate1", "Alternate2"};
const QString ALTERNATE_STRING_LIST_AS_STRING("AAAAAgAAABQAYQBsAHQAZQByAG4AYQB0AGUAMQAAABQAQQBsAHQAZQByAG4AYQB0AGUAMg==");

const QString STRING_DOUBLE_MAP_KEY("TestStringDoubleMap");
const QMap<QString, qreal> STRING_DOUBLE_MAP_VALUE{{"one", 1.}, {"onehalf", 1.5}, {"quarter", 0.25}, {"minushalf", -.5}};
const QVariantMap STRING_DOUBLE_MAP_VARIANT{{"one", 1.}, {"onehalf", 1.5}, {"quarter", 0.25}, {"minushalf", -.5}};
const QString STRING_DOUBLE_MAP_AS_STRING("{\"minushalf\":-0.5,\"one\":1,\"onehalf\":1.5,\"quarter\":0.25}");
const QMap<QString, qreal> ALTERNATE_STRING_DOUBLE_MAP_VALUE{{"two", 2.}, {"threehalf", 3.5}};
const QVariantMap ALTERNATE_STRING_DOUBLE_MAP_VARIANT{{"two", 2.}, {"threehalf", 3.5}};
const QString ALTERNATE_STRING_DOUBLE_MAP_AS_STRING("{\"threehalf\":3.5,\"two\":2}");

const QString STRING_INT_DOUBLE_MAP_KEY("TestStringIntDoubleMap");
const QMap<std::pair<QString, int>, qreal> STRING_INT_DOUBLE_MAP_VALUE{{{"one", 1}, 1.}, {{"onehalf", -3}, 1.5}, {{"one", 2}, 0.25}, {{"minushalf", 0}, -.5}};
const QVariantMap STRING_INT_DOUBLE_MAP_VARIANT{{"one", QVariantMap{{"1", 1.}, {"2", 0.25}}}, {"onehalf", QVariantMap{{"-3", 1.5}}}, {"minushalf", QVariantMap{{"0", -.5}}}};
const QString STRING_INT_DOUBLE_MAP_AS_STRING("[{\"keyInt\":0,\"keyString\":\"minushalf\",\"value\":-0.5},{\"keyInt\":1,\"keyString\":\"one\",\"value\":1},{\"keyInt\":2,\"keyString\":\"one\",\"value\":0.25},{\"keyInt\":-3,\"keyString\":\"onehalf\",\"value\":1.5}]");
const QMap<std::pair<QString, int>, qreal> ALTERNATE_STRING_INT_DOUBLE_MAP_VALUE{{{"two", 0}, 2.}, {{"threehalf", 1}, 3.5}};
const QVariantMap ALTERNATE_STRING_INT_DOUBLE_MAP_VARIANT{{"two", QVariantMap{{"0", 2.}}}, {"threehalf", QVariantMap{{"1", 3.5}}}};
const QString ALTERNATE_STRING_INT_DOUBLE_MAP_AS_STRING("[{\"keyInt\":1,\"keyString\":\"threehalf\",\"value\":3.5},{\"keyInt\":0,\"keyString\":\"two\",\"value\":2}]");

const QString STRING_KEY("TestStringKey");
const QString STRING_VALUE{"testString"};
const QString STRING_AS_STRING("testString");
const QString ALTERNATE_STRING{"alternate"};
const QString ALTERNATE_STRING_AS_STRING("alternate");

template<typename T, typename SETTINGS_ITEM_TYPE, typename SIGNAL_COUNTER_TYPE>
class SettingsItemTestCase : public CxxTest::TestSuite {
protected:
  QString key, valueAsString, alternateAsString, valueAsXml;
  T value, alternate;
  QVariant variant, alternateVariant;
  SettingsFacade *facade, *emptyFacade;
  SETTINGS_ITEM_TYPE *settingsItem, *emptySettingsItem;
  SIGNAL_COUNTER_TYPE *signalCounter;

  void readValue() {
    QS_ASSERT_EQUALS(settingsItem->get(), value);
    QS_ASSERT_EQUALS(settingsItem->getVariant(), variant);
    QS_ASSERT_EQUALS(settingsItem->serialize(), valueAsString);
  }

  void assertWritingXml() {
    QString actualXml;
    QXmlStreamWriter writer(&actualXml);
    settingsItem->writeXml(writer);
    QS_ASSERT_EQUALS(actualXml, valueAsXml);
  }

  void assertReadingXml() {
    QXmlStreamReader reader(valueAsXml);
    reader.readNextStartElement();
    emptySettingsItem->readXml(reader);
    QS_ASSERT_EQUALS(emptySettingsItem->get(), value);
  }

  template<typename U>
  void performSettingCycle(const U &newValue) {
    bool signalHandlerCalled = false;
    signalCounter->callback = [&] (const T &value) {
      TSM_ASSERT("Settings handler called recursively!", !signalHandlerCalled);
      if(!signalHandlerCalled) { signalHandlerCalled = true; settingsItem->set(value); }
    };
    QS_ASSERT_NOT_EQUALS(settingsItem->get(), alternate)

    settingsItem->set(newValue);
    QS_ASSERT_EQUALS(facade->value(key), alternateVariant)
    QS_ASSERT_EQUALS(settingsItem->get(), alternate)
    QS_ASSERT_EQUALS(settingsItem->getVariant(), alternateVariant)
    QS_ASSERT_EQUALS(settingsItem->serialize(), alternateAsString)
    signalCounter->assertPayloads({alternate});
  }

  SettingsItemTestCase(const QString& key, T value, T alternate, const QString &valueAsString, const QString &alternateAsString)
    : key(key),
      valueAsString(valueAsString),
      alternateAsString(alternateAsString),
      valueAsXml("<" + key + " value=\"" +valueAsString + "\"/>"),
      value(value),
      alternate(alternate),
      variant(QVariant(value)),
      alternateVariant(QVariant(alternate))
  {}

  SettingsItemTestCase(const QString& key, T value, T alternate, const QString &valueAsString, const QString &alternateAsString, const QVariant &variant, const QVariant &alternateVariant)
    : key(key),
      valueAsString(valueAsString),
      alternateAsString(alternateAsString),
      valueAsXml("<" + key + " value=\"" +valueAsString.toHtmlEscaped() + "\"/>"), // TODO escape xml
      value(value),
      alternate(alternate),
      variant(variant),
      alternateVariant(alternateVariant)
  {}

public:
  void setUp() override {
    facade = SettingsFacade::transientSettings();
    emptyFacade = SettingsFacade::transientSettings();
    settingsItem = new SETTINGS_ITEM_TYPE(key, facade);
    emptySettingsItem = new SETTINGS_ITEM_TYPE(key, emptyFacade);
    signalCounter = new SIGNAL_COUNTER_TYPE;

    facade->setValue(key, variant);
    QObject::connect(settingsItem, &SETTINGS_ITEM_TYPE::updated, signalCounter, &SIGNAL_COUNTER_TYPE::record);
  }

  void tearDown() override {
    delete settingsItem;
    delete emptySettingsItem;
    delete facade;
    delete emptyFacade;
    delete signalCounter;
  }
};

class DoubleSettingsItemUnitTest : public SettingsItemTestCase<qreal, DoubleSettingsItem, QRealSignalCounter> {
public:
  DoubleSettingsItemUnitTest() : SettingsItemTestCase(DOUBLE_KEY, DOUBLE_VALUE, ALTERNATE_DOUBLE, DOUBLE_AS_STRING, ALTERNATE_DOUBLE_AS_STRING) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};

class BoolSettingsItemUnitTest : public SettingsItemTestCase<bool, BoolSettingsItem, BoolSignalCounter> {
public:
  BoolSettingsItemUnitTest() : SettingsItemTestCase(BOOL_KEY, BOOL_VALUE, ALTERNATE_BOOL, BOOL_AS_STRING, ALTERNATE_BOOL_AS_STRING) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
  void testSettingIsCaseInsensitive() {
    performSettingCycle(QString("False"));
    signalCounter->callback = nullptr;
    TS_ASSERT(!settingsItem->get());
    settingsItem->set(QString("TRUE"));
    TS_ASSERT(settingsItem->get());
  }
};

class ColorSettingsItemUnitTest : public SettingsItemTestCase<QColor, ColorSettingsItem, ColorSignalCounter> {
public:
  ColorSettingsItemUnitTest() : SettingsItemTestCase(COLOR_KEY, COLOR_VALUE, ALTERNATE_COLOR, COLOR_AS_STRING, ALTERNATE_COLOR_AS_STRING) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};

class FontSettingsItemUnitTest : public SettingsItemTestCase<QFont, FontSettingsItem, FontSignalCounter> {
public:
  FontSettingsItemUnitTest() : SettingsItemTestCase(FONT_KEY, FONT_VALUE, ALTERNATE_FONT, FONT_AS_STRING, ALTERNATE_FONT_AS_STRING) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};

class StringListSettingsItemUnitTest : public SettingsItemTestCase<QStringList, StringListSettingsItem, StringListSignalCounter> {
public:
  StringListSettingsItemUnitTest() : SettingsItemTestCase(STRING_LIST_KEY, STRING_LIST_VALUE, ALTERNATE_STRING_LIST, STRING_LIST_AS_STRING, ALTERNATE_STRING_LIST_AS_STRING) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};

class StringDoubleMapSettingsItemUnitTest : public SettingsItemTestCase<QMap<QString, qreal>, StringDoubleMapSettingsItem, StringDoubleMapSignalCounter> {
public:
  StringDoubleMapSettingsItemUnitTest() : SettingsItemTestCase(STRING_DOUBLE_MAP_KEY, STRING_DOUBLE_MAP_VALUE, ALTERNATE_STRING_DOUBLE_MAP_VALUE, STRING_DOUBLE_MAP_AS_STRING, ALTERNATE_STRING_DOUBLE_MAP_AS_STRING, STRING_DOUBLE_MAP_VARIANT, ALTERNATE_STRING_DOUBLE_MAP_VARIANT) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};

class StringIntDoubleMapSettingsItemUnitTest : public SettingsItemTestCase<QMap<std::pair<QString, int>, qreal>, StringIntDoubleMapSettingsItem, StringIntDoubleMapSignalCounter> {
public:
  StringIntDoubleMapSettingsItemUnitTest() : SettingsItemTestCase(STRING_INT_DOUBLE_MAP_KEY, STRING_INT_DOUBLE_MAP_VALUE, ALTERNATE_STRING_INT_DOUBLE_MAP_VALUE, STRING_INT_DOUBLE_MAP_AS_STRING, ALTERNATE_STRING_INT_DOUBLE_MAP_AS_STRING, STRING_INT_DOUBLE_MAP_VARIANT, ALTERNATE_STRING_INT_DOUBLE_MAP_VARIANT) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};

class StringSettingsItemUnitTest : public SettingsItemTestCase<QString, StringSettingsItem, StringSignalCounter> {
public:
  StringSettingsItemUnitTest() : SettingsItemTestCase(STRING_KEY, STRING_VALUE, ALTERNATE_STRING, STRING_AS_STRING, ALTERNATE_STRING_AS_STRING) {}
  void testReading() { readValue(); }
  void testWritingXml() { assertWritingXml(); }
  void testReadingXml() { assertWritingXml(); }
  void testSettingValue() { performSettingCycle(alternate); }
  void testSettingVariant() { performSettingCycle(alternateVariant); }
  void testSettingString() { performSettingCycle(alternateAsString); }
};
