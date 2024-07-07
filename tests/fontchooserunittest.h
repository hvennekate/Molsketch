/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate                               *
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

#include "signalcounter.h"
#include "utilities.h"
#include <cxxtest/TestSuite.h>
#include <QCheckBox>
#include <QFontComboBox>
#include <QSpinBox>
#include <fontchooser.h>

using namespace Molsketch;

const int &FONT_SIZE = 25;
const int &FONT_INDEX = 5;
const char* INITIAL_VALUE = "Initial value should be different from test value";

class FontChooserUnitTest : public CxxTest::TestSuite {
  FontChooser *chooser;
  FontSignalCounter *signalCounter;
  QFontComboBox *fontName;
  QSpinBox *fontSize;
  QCheckBox *bold, *italic;

public:
  void setUp() override {
    chooser = new FontChooser();
    signalCounter = new FontSignalCounter(chooser);
    QObject::connect(chooser, SIGNAL(fontChanged(QFont)), signalCounter, SLOT(record(QFont)));

    fontName = assertNotNull(chooser->findChild<QFontComboBox*>("fontName"));
    fontSize = assertNotNull(chooser->findChild<QSpinBox*>("size"));
    bold = assertNotNull(chooser->findChild<QCheckBox*>("bold"));
    italic = assertNotNull(chooser->findChild<QCheckBox*>("italic"));
  }

  void tearDown() override {
    delete chooser;
  }

  void testFontSizeChange() {
    TSM_ASSERT_DIFFERS(INITIAL_VALUE, fontSize->value(), FONT_SIZE);
    fontSize->setValue(FONT_SIZE);
    TS_ASSERT_EQUALS(signalCounter->getLatestPayload().pointSize(), FONT_SIZE);
    TS_ASSERT_EQUALS(chooser->getSelectedFont().pointSize(), FONT_SIZE);
  }

  void testFontNameChange() {
    TSM_ASSERT_DIFFERS(INITIAL_VALUE, fontName->currentIndex(), FONT_INDEX);
    fontName->setCurrentIndex(FONT_INDEX);
    QS_ASSERT_EQUALS(signalCounter->getLatestPayload().family(), fontName->currentText());
    QS_ASSERT_EQUALS(chooser->getSelectedFont().family(), fontName->currentText());
  }

  void testSettingBold() {
    TSM_ASSERT_DIFFERS(INITIAL_VALUE, bold->isChecked(), true);
    bold->setChecked(true);
    TS_ASSERT(signalCounter->getLatestPayload().bold());
    TS_ASSERT(chooser->getSelectedFont().bold())
  }

  void testSettingItalic() {
    TSM_ASSERT_DIFFERS(INITIAL_VALUE, italic->isChecked(), true);
    italic->setChecked(true);
    TS_ASSERT(signalCounter->getLatestPayload().italic());
    TS_ASSERT(chooser->getSelectedFont().italic());
  }

  void testSettingFontProgrammatically() {
    TS_SKIP("To be reenabled with proper fonts");
    TS_ASSERT_DIFFERS(fontName->currentText(), "Times New Roman");
    TS_ASSERT_DIFFERS(fontSize->value(), FONT_SIZE);
    TS_ASSERT(!bold->isChecked());
    TS_ASSERT(!italic->isChecked());

    QFont newFont("Times New Roman", FONT_SIZE, QFont::Bold, true);
    chooser->setFont(newFont);

    signalCounter->assertPayloads({newFont});
    TS_ASSERT_EQUALS(fontName->currentText(), "Times New Roman");
    TS_ASSERT_EQUALS(fontSize->value(), FONT_SIZE);
    TS_ASSERT(bold->isChecked());
    TS_ASSERT(italic->isChecked());
  }
};
