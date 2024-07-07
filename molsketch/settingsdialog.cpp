/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 by Tim Vandermeersch                               *
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

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QFontDialog>
#include <molscene.h>
#include <scenesettings.h>
#include <settingsitem.h>

#include "settingsdialog.h"
#include "scenepropertieswidget.h"
#include "settingsfacade.h"

// TODO this is a make-shift solution to enable resetting to default values!
void setupDrawingSettings(Molsketch::SceneSettings *settings, QWidget *drawingPage) {
  QLayout *drawingPageLayout = drawingPage->layout();
  while (drawingPageLayout->count()) {
    auto layoutItem = drawingPageLayout->takeAt(0);
    delete layoutItem->widget();
    delete layoutItem;
  }
  auto sceneProperties = new Molsketch::ScenePropertiesWidget(settings, drawingPage);
  sceneProperties->setEnabled(true); // TODO this should be enabled by default
  drawingPageLayout->addWidget(sceneProperties);
}

SettingsDialog::SettingsDialog(ApplicationSettings *settings, QWidget * parent, Qt::WindowFlags f )
  : QDialog(parent,f),
    settings(settings),
    sceneSettingsFacade(const_cast<const ApplicationSettings*>(settings)->settingsFacade().cloneTransiently()) // TODO ownership?
{
  ui.setupUi(this);
  for (auto mainIcon : ui.listWidget->findItems("", Qt::MatchContains))
    mainIcon->setSizeHint(QSize(100, 60)); // TODO fix in ui XML

  QWidget *drawingPage = findChild<QWidget*>("drawingPage");
  setupDrawingSettings(new Molsketch::SceneSettings(sceneSettingsFacade, drawingPage), drawingPage);

  foreach(QListWidgetItem * item, ui.listWidget->findItems("*",Qt::MatchWildcard))
    item->setTextAlignment(Qt::AlignHCenter);
  ui.listWidget->setCurrentRow(0);

  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
  connect(ui.buttonBox, SIGNAL(helpRequested()), this, SLOT(showHelp()));

  setInitialValues();
}

SettingsDialog::~SettingsDialog(){}

void SettingsDialog::setInitialValues()
{
  ui.spinBoxAutoSave->setValue(settings->autoSaveInterval()/60000);
  // TODO image format setting!
  ui.pixelScalingFactor->setValue(settings->pixelScalingFactor());

  Molsketch::SceneSettings::MouseWheelMode mouseWheelForTools = settings->getMouseWheelMode();
  ui.mouseWheelCycleTools->setChecked(Molsketch::SceneSettings::CycleTools == mouseWheelForTools);
  ui.mouseWheelCycleTools->setChecked(Molsketch::SceneSettings::Zoom == mouseWheelForTools);

  ui.libraries->clear();
  ui.libraries->addItems(settings->libraries()->get());

  ui.libraryPath->setText(settings->obabelIfacePath());
  ui.obfPath->setText(settings->obabelFormatsPath());
  ui.wikidataQueryUrl->setText(settings->wikiQueryUrl()->get());
  // TODO accceptance test
}

void SettingsDialog::accept()
{
  applyChanges();
  QDialog::accept();
}

void SettingsDialog::applyChanges()
{
  settings->setAutoSaveInterval(ui.spinBoxAutoSave->value()*60000);
  settings->setPixelScalingFactor(ui.pixelScalingFactor->value());

  // Library settings
  QStringList libraries;
  for (int i = 0 ; i < ui.libraries->count() ; ++i)
    libraries << ui.libraries->item(i)->text();
  settings->libraries()->set(libraries);

  if (ui.mouseWheelCycleTools->isChecked())
    settings->setMouseWheelMode(ApplicationSettings::MouseWheelMode::CycleTools);
  else if (ui.mouseWheelZoom->isChecked())
    settings->setMouseWheelMode(ApplicationSettings::MouseWheelMode::Zoom);
  else
    settings->setMouseWheelMode(ApplicationSettings::MouseWheelMode::Unset);

  settings->setObabelIfacePath(ui.libraryPath->text());
  settings->setObabelFormatsPath(ui.obfPath->text());
  settings->wikiQueryUrl()->set(ui.wikidataQueryUrl->text());

  settings->transferFrom(*sceneSettingsFacade);

  emit settingsChanged();
}

void SettingsDialog::buttonClicked(QAbstractButton * button)
{
  switch (ui.buttonBox->standardButton(button)) {
    case QDialogButtonBox::RestoreDefaults:
      /* TODO make this really the default values instead of the initial values */
      setInitialValues();
      break;
    case QDialogButtonBox::Apply:
      applyChanges();
      break;
    default:
      break;
  }
}

void SettingsDialog::showHelp()
{
  /* TODO this should show the right help page */
}

void SettingsDialog::on_addLibrary_clicked()
{
  QString newLibrary = QFileDialog::getExistingDirectory(this, tr("Select library folder"));
  if (newLibrary.isEmpty()) return;
  ui.libraries->addItem(newLibrary);
  ui.libraries->sortItems();
}

void SettingsDialog::on_removeLibrary_clicked()
{
  foreach (QListWidgetItem* item, ui.libraries->selectedItems())
    delete item;
}

void SettingsDialog::on_obfPathButton_clicked() {
  QString folder = QFileDialog::getExistingDirectory(0, tr("OBF folder"), ui.obfPath->text());
  if (!folder.isEmpty()) ui.obfPath->setText(folder);
}

void SettingsDialog::on_libraryPathButton_clicked() {
  QString filename = QFileDialog::getOpenFileName(0, tr("Path to obabelIface"), ui.libraryPath->text(),
#ifdef Q_OS_WINDOWS
          "*.dll (*.dll)"
#elif defined Q_OS_UNIX
          "*.so (*.so)"
#else
          ""
#endif
                                                  );
  if (!filename.isEmpty()) ui.libraryPath->setText(filename);
}
