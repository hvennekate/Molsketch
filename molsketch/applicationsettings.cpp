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

#include "applicationsettings.h"
#include "programversion.h"

#include <QFile>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QDir>
#include <settingsfacade.h>
#include <settingsitem.h>

#define PROPERTY_DEF(TYPE, NAME) \
  const TYPE* ApplicationSettings::NAME() const { return d_ptr->NAME; } \
  TYPE* ApplicationSettings::NAME() { return d_ptr->NAME; }

static const char LATEST_RELEASE_NOTES_VERSION_SHOWN[] = "latest-release-notes";
static const char VERSION_FILE[] = ":/version";
static const char VERSION_NICK_FILE[] = ":/versionnick";
static const char LIBRARIES[] = "libraries";
static const char ICON_STYLE[] = "tool-bar-icon-style";
static const char WINDOW_SIZE[] = "size";
static const char WINDOW_STATE[] = "window-state";
static const char AUTO_SAVE_INTERVAL[] = "auto-save-time";
static const char LAST_SAVE_PATH[] = "last-save-path";
static const char OBABEL_IFACE[] = "obabel-iface-path";
static const char OBABEL_FORMATS[] = "obabel-formats-dir";
static const char PIXEL_SCALING_FACTOR[] = "pixel-scaling-factor";
static const char WIKIDATA_QUERY_URL[] = "wikidata-query-url";

static const int& DEFAULT_AUTO_SAVE_INTERVAL = 300000;
static const QByteArray& DEFAULT_WINDOW_STATE = QByteArray("@ByteArray(\0\0\0\xff\0\0\0\0\xfd\0\0\0\x1\0\0\0\0\0\0\x1\xe\0\0\x3N\xfc\x2\0\0\0\x2\xfb\0\0\0$\0t\0o\0o\0l\0\x62\0o\0x\0-\0\x64\0o\0\x63\0k\0w\0i\0\x64\0g\0\x65\0t\x1\0\0\0x\0\0\x2\x65\0\0\0\xe8\0\xff\xff\xff\xfb\0\0\0$\0i\0n\0\x66\0o\0\x62\0o\0x\0-\0\x64\0o\0\x63\0k\0w\0i\0\x64\0g\0\x65\0t\x1\0\0\x2\xe3\0\0\0\xe3\0\0\0l\0\xff\xff\xff\0\0\x6l\0\0\x3N\0\0\0\x1\0\0\0\x4\0\0\0\x1\0\0\0\b\xfc\0\0\0\x2\0\0\0\x2\0\0\0\x3\0\0\0\x18\0\x66\0i\0l\0\x65\0-\0t\0o\0o\0l\0\x62\0\x61\0r\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x18\0\x65\0\x64\0i\0t\0-\0t\0o\0o\0l\0\x62\0\x61\0r\x1\0\0\x1\x33\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x18\0z\0o\0o\0m\0-\0t\0o\0o\0l\0\x62\0\x61\0r\x1\0\0\x2s\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x2\0\0\0\x4\0\0\0\b\0\x44\0r\0\x61\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\n\0R\0i\0n\0g\0s\x1\0\0\x2G\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\n\0T\0o\0o\0l\0s\x1\0\0\x3t\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x10\0R\0\x65\0\x61\0\x63\0t\0i\0o\0n\x1\0\0\x4\x80\xff\xff\xff\xff\0\0\0\0\0\0\0\0)");
static const QString& DEFAULT_LAST_SAVE_PATH = QDir::homePath();
static const qreal& DEFAULT_PIXEL_SCALING_FACTOR = 10;
static const char DEFAULT_WIKIDATA_URL[] = "https://query.wikidata.org/sparql";

QString readFileContent(const QString& absolutePath) {
  QFile file(absolutePath);
  file.open(QFile::ReadOnly);
  return file.readAll();
}

using namespace Molsketch;

class ApplicationSettingsPrivate {
  Q_DISABLE_COPY(ApplicationSettingsPrivate)
public:
  StringListSettingsItem *libraries;
  StringSettingsItem *wikiQueryUrl;
  ApplicationSettingsPrivate(SettingsFacade *facade, QObject *parent) {
    libraries = new StringListSettingsItem(LIBRARIES, facade, parent);
    wikiQueryUrl = new StringSettingsItem(WIKIDATA_QUERY_URL, facade, parent, DEFAULT_WIKIDATA_URL);
  }
};

ApplicationSettings::ApplicationSettings(SettingsFacade *facade, QObject *parent)
  : SceneSettings(facade, parent), d_ptr(new ApplicationSettingsPrivate(facade, this)) {}

ApplicationSettings::~ApplicationSettings() {}

ProgramVersion ApplicationSettings::latestReleaseNotesVersionShown() const {
  return ProgramVersion(settingsFacade().value(LATEST_RELEASE_NOTES_VERSION_SHOWN).toString());
}

ProgramVersion ApplicationSettings::currentVersion() const {
  return ProgramVersion(readFileContent(VERSION_FILE));
}

void ApplicationSettings::updateReleaseNotesShownVersion() {
  settingsFacade().setValue(LATEST_RELEASE_NOTES_VERSION_SHOWN, currentVersion().toString());
}

QString ApplicationSettings::versionNick() const {
  return readFileContent(VERSION_NICK_FILE);
}

APP_PROPERTY(ToolButtonStyle, Qt::ToolButtonStyle, ICON_STYLE)
APP_PROPERTY(WindowSize, QSize, WINDOW_SIZE)

PROPERTY_DEF(StringListSettingsItem, libraries)

void ApplicationSettings::setWindowState(const QByteArray &state) {
  settingsFacade().setValue(WINDOW_STATE, state);
}

QByteArray ApplicationSettings::windowState() const {
  return settingsFacade().value(WINDOW_STATE, DEFAULT_WINDOW_STATE).toByteArray();
}

void ApplicationSettings::setLastPath(const QString &path) {
  settingsFacade().setValue(LAST_SAVE_PATH, path);
}

QString ApplicationSettings::lastPath() const {
  return settingsFacade().value(LAST_SAVE_PATH, DEFAULT_LAST_SAVE_PATH).toString();
}

void ApplicationSettings::setAutoSaveInterval(const int &interval) {
  settingsFacade().setValue(AUTO_SAVE_INTERVAL, interval);
}

int ApplicationSettings::autoSaveInterval() const {
  return settingsFacade().value(AUTO_SAVE_INTERVAL, DEFAULT_AUTO_SAVE_INTERVAL).toInt();
}

void ApplicationSettings::setObabelIfacePath(const QString &path) {
  settingsFacade().setValue(OBABEL_IFACE, path);
  emit changedObabelIfacePath(path);
}

QString ApplicationSettings::obabelIfacePath() const {
  return settingsFacade().value(OBABEL_IFACE, OBABEL_IFACE_LOCATION).toString();
}

void ApplicationSettings::setObabelFormatsPath(const QString &path) {
  settingsFacade().setValue(OBABEL_FORMATS, path);
  emit changeObabelFormatsPath(path);
}

QString ApplicationSettings::obabelFormatsPath() const {
  return settingsFacade().value(OBABEL_FORMATS).toString();
}

void ApplicationSettings::setPixelScalingFactor(const qreal &scalingFactor) {
  settingsFacade().setValue(PIXEL_SCALING_FACTOR, scalingFactor);
}

qreal ApplicationSettings::pixelScalingFactor() const {
  return settingsFacade().value(PIXEL_SCALING_FACTOR, DEFAULT_PIXEL_SCALING_FACTOR).toDouble();
}

PROPERTY_DEF(StringSettingsItem, wikiQueryUrl)

