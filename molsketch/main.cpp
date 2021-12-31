/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
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


#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <settingsfacade.h>
#include <QSettings>
#include <QCommandLineParser>
#include <QDebug>

#include "applicationsettings.h"
#include "mainwindow.h"
#include "programversion.h"
#include "releasenotesdialog.h"

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: (%s:%u, %s) %s\n", context.file, context.line, context.function, localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: (%s:%u, %s) %s\n", context.file, context.line, context.function, localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: (%s:%u, %s) %s\n", context.file, context.line, context.function, localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: (%s:%u, %s) %s\n", context.file, context.line, context.function, localMsg.constData());
        abort();
      case QtInfoMsg:
        fprintf(stderr, "Info: (%s:%u, %s) %s\n", context.file, context.line, context.function, localMsg.constData());
    }
}

int main(int argc, char *argv[])
{
  qInstallMessageHandler(messageOutput);
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("SourceForge");
  QCoreApplication::setOrganizationDomain("sourceforge.net");
  QCoreApplication::setApplicationName("Molsketch");

  QTranslator molsketchTranslator, libraryTranslator;
  auto molsketchTranslationsLoaded = molsketchTranslator.load(QLocale::system(), "molsketch", "_", ":/i18n"); // TODO also load Qt module translations
  auto libraryTranslationsLoaded = libraryTranslator.load(QLocale::system(), "libmolsketch", "_", ":/i18n");
  app.installTranslator(&molsketchTranslator);
  app.installTranslator(&libraryTranslator);
  qDebug() << "System locale:" << QLocale::system() << "Translation loaded:" << molsketchTranslationsLoaded << "for library:" << libraryTranslationsLoaded;

  QCommandLineParser parser;
  parser.addPositionalArgument("files", QApplication::translate("main", "Files to open, optionally."), "[files...]");
  parser.process(app);
  auto filesToOpen = parser.positionalArguments();
  for(auto filename : filesToOpen) (new MainWindow())->openFile(filename);
  if (filesToOpen.isEmpty()) new MainWindow();

  ApplicationSettings appSettings(Molsketch::SettingsFacade::persistedSettings(new QSettings));

  if (appSettings.latestReleaseNotesVersionShown() < appSettings.currentVersion()) {
    ReleaseNotesDialog().exec();
    appSettings.updateReleaseNotesShownVersion();
  }

  return app.exec();
}
