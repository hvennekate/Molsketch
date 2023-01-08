/***************************************************************************
 *   Copyright (C) 2018 Hendrik Vennekate                                  *
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
#include "licensedialog.h"
#include "ui_licensedialog.h"
#include <QFile>
#include <QFontDatabase>

QString readFile(const QString filename) {
  QFile input(filename);
  input.open(QIODevice::ReadOnly);
  return input.readAll();
}

LicenseDialog::LicenseDialog(QWidget *parent)
  : QDialog(parent),
    internal(new Ui::LicenseDialog)
{
  internal->setupUi(this);
  // TODO write test to check that license texts are actually present
  auto gpl2 = readFile(":/licenses/gpl2.html");
  internal->licenseTextCppRuntime->setHtml(gpl2);
  internal->licenseTextOpenBabel->setHtml(gpl2);
  auto lgpl3 = readFile(":/licenses/lgpl3.html");
  internal->licenseTextQt->setHtml(lgpl3);

  auto fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  auto winpthreads = readFile(":/licenses/winpthreads.txt");
  internal->licenseTextWinpthreads->setFont(fixedFont);
  internal->licenseTextWinpthreads->setPlainText(winpthreads);
  auto zlib = readFile(":/licenses/zlib.txt");
  internal->licenseTextZlib->setFont(fixedFont);
  internal->licenseTextZlib->setPlainText(zlib);
}

LicenseDialog::~LicenseDialog() {}

void LicenseDialog::on_aboutQtButton_clicked() {
  qApp->aboutQt();
}
