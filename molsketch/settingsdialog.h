/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 by Tim Vandermeersch                               *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "applicationsettings.h"
#include "ui_settingsdialog.h"

#include <QDialog>

namespace Molsketch {
  class SettingsFacade;
}


class SettingsDialog : public QDialog
  {
  Q_OBJECT

  public:
    SettingsDialog(ApplicationSettings* settings, QWidget * parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    ~SettingsDialog();

signals:
    void settingsChanged();

  private:
    ApplicationSettings* settings;
    Ui::SettingsDialog ui; // TODO use pointer instead
    Molsketch::SettingsFacade *sceneSettingsFacade;

  private slots:
    void accept() override;
    void applyChanges();
    void setInitialValues();
    void buttonClicked(QAbstractButton * button);
    void showHelp();
    void on_addLibrary_clicked();
    void on_removeLibrary_clicked();
    void on_obfPathButton_clicked();
    void on_libraryPathButton_clicked();
};

#endif
