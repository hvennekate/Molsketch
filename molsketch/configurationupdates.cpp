/***************************************************************************
 *   Copyright (C) 2021 Hendrik Vennekate                                  *
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
#include "applicationsettings.h"
#include "programversion.h"
#include "configurationupdates.h"
#include "pathinput.h"
#include "settingsfacade.h"

#include <QFileInfo>
#include <QFormLayout>
#include <QSettings>
#include <QDir>

static const char QT_SUFFIX[] = "-qt5";

ConfigurationUpdates::ConfigurationUpdates(QWidget *parent)
  : QWidget(parent) {
  auto layout = new QFormLayout(this);
  setLayout(layout);
  auto appSettings = new ApplicationSettings(Molsketch::SettingsFacade::persistedSettings(new QSettings), this);
  auto originalObabelIfacePath = QFileInfo(appSettings->obabelIfacePath());

  if (appSettings->latestReleaseNotesVersionShown() < ProgramVersion(0,7,2)
      && originalObabelIfacePath.fileName().contains(QT_SUFFIX)) {
    auto obabelIfacePathWidget = PathInput::fileInput(
#ifdef Q_OS_WINDOWS
          "*.dll (*.dll)"
#elif defined Q_OS_UNIX
          "*.so (*.so)"
#else
          ""
#endif
          , this);
    connect(obabelIfacePathWidget, &PathInput::pathStringChanged, appSettings, &ApplicationSettings::setObabelIfacePath);
    auto newObabelIfacePath = QFileInfo(originalObabelIfacePath.dir(), originalObabelIfacePath.fileName().remove(QT_SUFFIX)).filePath();
    auto label = tr("OpenBabel Library Path");
    obabelIfacePathWidget->setLabel(label);
    obabelIfacePathWidget->setValue(newObabelIfacePath);
    layout->addRow(label, obabelIfacePathWidget);
  }
}
