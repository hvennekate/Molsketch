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

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>
#include "scenesettings.h"

class ProgramVersion;
class QByteArray;

namespace Molsketch {
  class StringListSettingsItem;
  class StringSettingsItem;
}

class ApplicationSettingsPrivate;

class ApplicationSettings : public Molsketch::SceneSettings // TODO this doesn't seem to be such a very good idea... better have these settings independent of the scene settings
{
  Q_OBJECT
  Q_DECLARE_PRIVATE(ApplicationSettings)
  QScopedPointer<ApplicationSettingsPrivate> d_ptr;
public:
  explicit ApplicationSettings(Molsketch::SettingsFacade *facade, QObject *parent = 0);
  ~ApplicationSettings();

  ProgramVersion latestReleaseNotesVersionShown() const;
  ProgramVersion currentVersion() const;
  void updateReleaseNotesShownVersion();
  QString versionNick() const;
  PROPERTY_DECL(Molsketch::StringListSettingsItem, libraries)
  void setToolButtonStyle(const Qt::ToolButtonStyle&);
  Qt::ToolButtonStyle getToolButtonStyle() const;
  void setWindowSize(const QSize&);
  QSize getWindowSize() const;
  void setWindowState(const QByteArray&);
  QByteArray windowState() const;
  void setLastPath(const QString&);
  QString lastPath() const;
  void setAutoSaveInterval(const int&);
  int autoSaveInterval() const;
  void setObabelIfacePath(const QString&);
  QString obabelIfacePath() const;
  void setObabelFormatsPath(const QString&);
  QString obabelFormatsPath() const;
  void setPixelScalingFactor(const qreal&);
  qreal pixelScalingFactor() const;
  PROPERTY_DECL(Molsketch::StringSettingsItem, wikiQueryUrl)
signals:
  void changedObabelIfacePath(const QString&);
  void changeObabelFormatsPath(const QString&);
};

#define APP_PROPERTY(NAME, TYPE, CONFIGSTRING) \
  void ApplicationSettings::set##NAME(const TYPE& value) { settingsFacade().setValue(CONFIGSTRING, value); } \
  TYPE ApplicationSettings::get##NAME() const { return settingsFacade().value(CONFIGSTRING).value<TYPE>(); }


#endif // APPLICATIONSETTINGS_H
