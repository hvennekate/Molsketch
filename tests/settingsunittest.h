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

#include <tempfileprovider.h>
#include <QDebug>
#include <QTemporaryFile>
#include <scenesettings.h>
#include <settingsfacade.h>
#include <settingsitem.h>
#include <QSettings>
#include <QRegularExpression>
#include "xmlassertion.h"
#include "utilities.h"

using namespace Molsketch;
using XmlAssert::assertThat;

const QString SERIALIZED_SETTINGS("");
const QFont ATOM_FONT("Helvetica", 15, QFont::Cursive);
const qreal BOND_ANGLE = 1.25;
const QString BASE64_ATOM_FONT("AAAAEgBIAGUAbAB2AGUAdABpAGMAYf////9ALgAAAAAAAP////8FAAEABhAAAAEAAAAAAAAAAAAAAAAAAA==");
const QString BOND_ANGLE_NAME("bond-angle");
const QString BOND_ANGLE_OLD_NAME("MolsceneBondAngle");
const QString ATOM_FONT_NAME("atom-font");
const QString ATOM_FONT_OLD_NAME("MolsceneAtomFont");
const QString SETTINGS_XML("<settings>"
                           "<" + BOND_ANGLE_NAME + " value=\"25\"/>"
                           "<" + ATOM_FONT_NAME + " value=\"" + BASE64_ATOM_FONT + "\"/>"
                           "</settings>");


class SettingsUnitTest : public TempFileProvider, public CxxTest::TestSuite {
  const QString SETTINGS_FILE_CONTENT = "bond-angle=25";
  const qreal BOND_ANGLE = 25;
  const qreal DEFAULT_BOND_ANGLE = 30;
  SceneSettings *settings;

public:
  void setUp() {
    settings = nullptr;
  }

  void tearDown() {
    delete settings;
  }

  void testSceneSettingsArePersisted() {
    QFile *settingsFile = createTemporaryFile();
    settingsFile->open(QFile::ReadOnly);
    settings = new SceneSettings(SettingsFacade::persistedSettings(new QSettings(settingsFile->fileName(), QSettings::IniFormat)));
    settings->bondAngle()->set(BOND_ANGLE);
    delete settings;
    settings = nullptr;
    QFile readFile(settingsFile->fileName());
    readFile.open(QFile::ReadOnly);
    QString fileContent = readFile.readAll();
    TS_ASSERT(fileContent.contains(SETTINGS_FILE_CONTENT)); // TODO QString assertions
  }

  void testPersistedSceneSettingsAreRead() {
    QFile *settingsFile = createTemporaryFile();
    settingsFile->open(QFile::WriteOnly);
    settingsFile->write(SETTINGS_FILE_CONTENT.toUtf8());
    settingsFile->close();
    SettingsFacade *facade = SettingsFacade::persistedSettings(new QSettings(settingsFile->fileName(), QSettings::IniFormat));
    settings = new SceneSettings(facade);
    TS_ASSERT_EQUALS(settings->bondAngle()->get(), BOND_ANGLE);
  }

  void testSceneSettingsReturnDefault() {
    QFile *settingsFile = createTemporaryFile();
    settingsFile->open(QFile::WriteOnly);
    settingsFile->close();
    settings = new SceneSettings(SettingsFacade::persistedSettings(new QSettings(settingsFile->fileName(), QSettings::IniFormat)));
    TS_ASSERT_EQUALS(settings->bondAngle()->get(), DEFAULT_BOND_ANGLE);
  }

  void testSettingsSerialization() {
    settings = new SceneSettings(SettingsFacade::transientSettings());
    settings->atomFont()->set(ATOM_FONT);
    settings->bondAngle()->set(BOND_ANGLE);
    QString actualXml;
    QXmlStreamWriter writer(&actualXml);
    settings->writeXml(writer);
    assertThat(actualXml)->contains("/settings/atom-font/@value/data(.)")->exactlyOnceWithContent(BASE64_ATOM_FONT);
    assertThat(actualXml)->contains("/settings/bond-angle/@value/data(.)")->exactlyOnceWithContent(QString::number(BOND_ANGLE));
  }

  void testSettingsDeserialization() {
    QXmlStreamReader reader(SETTINGS_XML);
    settings = new SceneSettings(SettingsFacade::transientSettings());
    settings->readXml(reader);
    QS_ASSERT_EQUALS(settings->bondAngle()->get(), BOND_ANGLE);
    QS_ASSERT_EQUALS(settings->atomFont()->get(), ATOM_FONT);
  }

  void testInitializationFromXmlAttributes() {
    QXmlStreamAttributes attributes;
    attributes.append(BOND_ANGLE_OLD_NAME, QString::number(BOND_ANGLE));
    attributes.append(ATOM_FONT_OLD_NAME, BASE64_ATOM_FONT);
    settings = new SceneSettings(SettingsFacade::transientSettings());
    settings->setFromAttributes(attributes);
    QS_ASSERT_EQUALS(settings->bondAngle()->get(), BOND_ANGLE);
    QS_ASSERT_EQUALS(settings->atomFont()->get(), ATOM_FONT);
  }

  // TODO mouse wheel settings test
  // TODO make sure no setting occurs twice (individual keys -> iterate over settings item list)
};
