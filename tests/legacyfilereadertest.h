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

#include <cxxtest/TestSuite.h>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>
#include <fileio.h>
#include <molscene.h>

using namespace Molsketch;

class LegacyFileReaderTest : public CxxTest::TestSuite {
public:
  void setUp() {

  }

  void tearDown() {
  }

  QByteArray fileContent(const QString & absoluteFilePath) {
    QFile file(absoluteFilePath);
    file.open(QFile::ReadOnly);
    return file.readAll();
  }

  void testFoundFiles() {
    TS_SKIP("To be redone");
    QDir legacyMskFiles("../tests/legacy", "*.msk");
    for (QFileInfo fileInfo : legacyMskFiles.entryInfoList()) {
      MolScene scene;
      QString mskFileName(fileInfo.absoluteFilePath());
      readMskFile(mskFileName, &scene);
      QString resultFileName(mskFileName + "-current.svg");
      saveToSVG(resultFileName, &scene);
      QByteArray result(fileContent(resultFileName)), original(fileContent(mskFileName.replace(QRegularExpression("msk$"), "svg")));
      TSM_ASSERT_EQUALS("Legacy file " + mskFileName + " not correctly rendered", original, result);
    }
  }
};
