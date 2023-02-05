/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
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

#include <fstream>

#include <QtGui>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QProgressBar>
#include <QPrintPreviewDialog>
#include <QMenuBar>
#include <actions/lineupaction.h>

#include <QToolTip>
#include <propertiesdock.h>
#include <settingsfacade.h>

#include "mainwindow.h"

#include "molview.h"
#include "fileio.h"
#include "programversion.h"
#include "indicator.h"
#include "obabelifaceloader.h"
#include "releasenotesdialog.h"
#include "settingsdialog.h"
#include "wikiquerywidget.h"
#include "constants.h"
#include "librarytoolbox.h"
#include "settingsitem.h"
#include "actioncontainer.h"
#include "sumformulaindicator.h"

#ifdef THIRD_PARTY_LICENSES
#include "licensedialog.h"
#endif


#define PROGRAM_NAME "Molsketch"

#define MSK_MSM_FORMATS "Molsketch (*.msk *.msm)"
#define MSK_FORMAT "Molsketch (*.msk)"
#define GRAPHIC_FILE_FORMATS "Scalable Vector Graphics (*.svg);;Portable Network Graphics (*.png);;Windows Bitmap (*.bmp);;Joint Photo Expert Group (*.jpeg)"
#define GRAPHIC_DEFAULT_FORMAT "Portable Network Graphics (*.png)"
#define OSRA_GRAPHIC_FILE_FORMATS "All supported types (*.*);;Images (*.png *.bmp *.jpg *.jpeg *.gif *.tif *.tiff);;Documents (*.pdf *.ps)"

#ifdef Q_OS_WINDOWS
#define OBABELOSSUFFIX ".dll"
#else
#define OBABELOSSUFFIX
#endif

const char EMPTY_FILENAME[] = "untitled.msk";
const char MSK_NATIVE_FORMAT[] = ".msk";
const char MSM_MOLECULE_FORMAT[] = ".msm";

using namespace Molsketch;

void setIconSizeOfAllToolBars(QObject* parent, QSize size) {
  foreach (QToolBar* bar, parent->findChildren<QToolBar*>())
    bar->setIconSize(size);
}

MainWindow::MainWindow()
  : MainWindow(new ApplicationSettings(SettingsFacade::persistedSettings(new QSettings)))
{}

MainWindow::MainWindow(ApplicationSettings *appSetttings)
  : m_autoSaveTimer(new QTimer(this)),
    settings(appSetttings),
    obabelLoader(new OBabelIfaceLoader(this))
{
  setAttribute(Qt::WA_DeleteOnClose);
  settings->setParent(this);
  setWindowIcon(QIcon(":/images/molsketch.svg"));

  m_molView = MolView::createView(const_cast<const ApplicationSettings*>(settings)->settingsFacade().cloneTransiently());
  setCentralWidget(m_molView);
  connect(m_molView->scene()->stack(), &QUndoStack::cleanChanged, this, &MainWindow::cleanChanged);

  actionContainer = new ActionContainer(m_molView, this);
  createFileMenuAndToolBar();

  menuBar()->addMenu(actionContainer->createEditMenu(this));
  addToolBar(actionContainer->createEditToolBar(this));

  menuBar()->addMenu(actionContainer->createViewMenu(this));
  addToolBar(actionContainer->createZoomToolBar(this));

  addToolBar(actionContainer->createDrawingToolBar(m_molView->scene(), this));
  addToolBar(actionContainer->createModifyToolBar(m_molView->scene(), this));
  addToolBar(actionContainer->createAlignmentToolBar(m_molView->scene(), obabelLoader, this));
  setIconSizeOfAllToolBars(this,
#ifdef __ANDROID__
                        QSize(48,48)
#else
                        QSize(22,22)
#endif
                        );
  actionContainer->addContextMenuActions(m_molView->scene());

  createHelpMenu();

  auto libraryDock = new LibraryToolBox(settings->libraries()->get());
  connect(settings->libraries(), SIGNAL(updated(QStringList)), libraryDock, SLOT(rebuildLibraries(QStringList)));
  addDockWidget(Qt::LeftDockWidgetArea, libraryDock);

  auto wikiQueryWidget = new WikiQueryWidget(obabelLoader, settings->wikiQueryUrl()->get(), this);
  addDockWidget(Qt::LeftDockWidgetArea, wikiQueryWidget);
  connect(settings->wikiQueryUrl(), &StringSettingsItem::updated, wikiQueryWidget, &WikiQueryWidget::setQueryUrl);

  auto propertiesDock = new Molsketch::PropertiesDock(m_molView);
  addDockWidget(Qt::LeftDockWidgetArea, propertiesDock);
  connect(m_molView->scene(), &MolScene::selectionChanged, propertiesDock, &PropertiesDock::selectionChanged);
  emit m_molView->scene()->selectionChanged();

  createStatusBar();
  createToolBarContextMenuOptions();

  readSettings();
  setCurrentFile("");
  connect(m_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));

  show();
}

QMenu *MainWindow::createPopupMenu() // TODO outsource
{
  QMenu* popupMenu = QMainWindow::createPopupMenu();
  if (!popupMenu) return popupMenu;
  popupMenu->addSeparator();
  QMenu *buttonStyleMenu = new QMenu(tr("Toolbar style"), popupMenu);
  buttonStyleMenu->addActions(toolBarTextsAndIcons->actions());
  popupMenu->addMenu(buttonStyleMenu);
  return popupMenu;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (!maybeSave()) {
    event->ignore();
    return;
  }
  saveWindowProperties();
  event->accept();
  deleteLater();
}

void MainWindow::newFile() {
  new MainWindow();
}

void MainWindow::open()
{
  QStringList readableFormats;
  readableFormats << MSK_MSM_FORMATS << obabelLoader->inputFormats();
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open"),
                                                  settings->lastPath(),
                                                  readableFormats.join(";;"));
  if (fileName.isEmpty()) return;

  openFile(fileName);
}

void MainWindow::readToSceneUsingOpenBabel(const QString& fileName) {
  std::ifstream inputFileStream(fileName.toStdString());
  if (!inputFileStream.is_open()) {
    qCritical() << "Could not open file. Filename: " + fileName;
    QMessageBox::critical(this, tr(PROGRAM_NAME), tr("Could not open file.")) ;
    return;
  }

  Molecule* mol = obabelLoader->loadFile(&inputFileStream,
                                         fileName.toStdString(),
                                         settings->bondLength()->get()); // TODO add coordinates if format does not supply them (e.g. InChI)
  if (!mol) {
    qCritical() << "Could not read file using OpenBabel. Filename: " + fileName;
    QMessageBox::critical(this, tr(PROGRAM_NAME), tr("Could not open file using OpenBabel.")) ;
    return;
  }

  // TODO encapsulate access to scene
  for(auto molecule : mol->split()) m_molView->scene()->addItem(molecule);
  delete mol;
}

void MainWindow::openFile(const QString& fileName) {
  if (isWindowModified() || !windowFilePath().isEmpty()) {
    (new MainWindow())->openFile(fileName);
    return;
  }

  settings->setLastPath(QFileInfo(fileName).path());
  if (fileName.endsWith(MSM_MOLECULE_FORMAT)) {
    QMessageBox::warning(this, tr("Molecule"), tr("File is an individual molecule,\nif you want to save it, use the molecule properties!"));
    readMskFile(fileName, m_molView->scene());
  } else {
    if (fileName.endsWith(MSK_NATIVE_FORMAT)) readMskFile(fileName, m_molView->scene());
    else readToSceneUsingOpenBabel(fileName);
    setCurrentFile(fileName);
  }

}

bool MainWindow::saveFile(const QString& fileName) {
  if (fileName.endsWith(".msk")) {
      if (!writeMskFile(fileName, m_molView->scene())) {
        QMessageBox::warning(this, tr("Saving file failed!"), tr("Could not save file '%1'.").arg(fileName));
        return false;
      }
  } else {
    bool threeD = QMessageBox::question(this, tr("Save as 3D?"), tr("Save as three dimensional coordinates?")) == QMessageBox::Yes;
    auto scene = m_molView->scene();
    std::ofstream output(fileName.toStdString());
    if (!output.is_open()) {
      QMessageBox::warning(0, tr("Could not save"), tr("Could not save file '%1'.").arg(fileName));
      return false ;
    }
    if (!obabelLoader->saveFile(&output,
                                fileName.toStdString(),
                                scene->molecules(),
                                threeD,
                                scene->settings()->autoAddHydrogen(),
                                scene->settings()->bondLength()->get())) {
      QMessageBox::warning(0, tr("Could not save"), tr("Could not save file '%1' using OpenBabel.").arg(fileName));
      return false ;
    }
  }
  m_molView->scene()->stack()->setClean();
  setCurrentFile(fileName);
  return true ;
}

bool MainWindow::save()
{
  if (windowFilePath().isEmpty()) return saveAs();
  return saveFile(windowFilePath());
}

bool MainWindow::autoSave()
{
  if(!isWindowModified()) return true;
  QFileInfo fileName(windowFilePath());


  if (!fileName.exists())
    fileName.setFile(QDir::homePath() + tr("/untitled.backup.msk"));
  else
    fileName.setFile(fileName.path() + fileName.baseName() +  ".backup." + fileName.completeSuffix());
  // And save the file
  if (fileName.suffix() == "msk") {
    bool saved = writeMskFile(fileName.absoluteFilePath(), m_molView->scene());
    statusBar()->showMessage(saved ? tr("Document autosaved") : tr("Autosave failed!"));
    return saved;
  } else {
    bool threeD = QMessageBox::question(this, tr("Save as 3D?"), tr("Save as three dimensional coordinates?")) == QMessageBox::Yes; // TODO not in autosave!
    auto scene = m_molView->scene();
    std::ofstream output(fileName.absoluteFilePath().toStdString());
    if (!output.is_open()) {
      statusBar()->showMessage(tr("Autosave failed! Could not open file '%1'.").arg(fileName.absoluteFilePath()), 10000);
      return false;
    }
    if (!obabelLoader->saveFile(&output,
                                fileName.fileName().toStdString(),
                                scene->molecules(),
                                threeD,
                                scene->settings()->autoAddHydrogen(),
                                scene->settings()->bondLength()->get())) {
      statusBar()->showMessage(tr("Autosave failed! OpenBabel unavailable."), 10000);
      return false ;
    }
  }
  statusBar()->showMessage(tr("Document autosaved"), 10000);
  return true;
}

bool MainWindow::saveAs() {
  QString filter = MSK_FORMAT;
  QStringList supportedFormats;
  supportedFormats << MSK_FORMAT << obabelLoader->outputFormats();
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save as"),
                                                  settings->lastPath(),
                                                  supportedFormats.join(";;"),
                                                  &filter);
  if (fileName.isEmpty()) return false;
  if (MSK_FORMAT == filter
      && QFileInfo(fileName).suffix().isEmpty()
      && !QFileInfo(fileName + MSK_NATIVE_FORMAT).exists())
    fileName += MSK_NATIVE_FORMAT;

  settings->setLastPath(QFileInfo(fileName).path());

  qDebug() << "Trying to save as " << fileName << "\n";

  return saveFile(fileName);
}



  bool MainWindow::importDoc()
  {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import"), settings->lastPath(), tr(OSRA_GRAPHIC_FILE_FORMATS));

    if (!fileName.isEmpty()) {
      // Save accessed path
      settings->setLastPath(QFileInfo(fileName).path());

      m_molView->scene()->clear(); // TODO this should actually end up in a new window!
      QProgressBar *pb = new QProgressBar(this);
      pb->setMinimum(0);
      pb->setMaximum(0);
      Molecule* mol = obabelLoader->callOsra(fileName, m_molView->scene()->settings()->bondLength()->get());
      if (mol) {
        if (mol->canSplit()) {
          QList<Molecule*> molList = mol->split();
          foreach(Molecule* mol,molList)
            m_molView->scene()->addItem(mol);
        } else {
          m_molView->scene()->addItem(mol);
        }

        setCurrentFile(fileName);
        return true;
      } else {
        QMessageBox::critical(this, tr(PROGRAM_NAME), tr("Error importing file"), QMessageBox::Ok, QMessageBox::Ok);
        return false;
      }

    }

    return false;
  }

bool MainWindow::exportDoc()
{
  // Getting the filename
  QString filter = GRAPHIC_DEFAULT_FORMAT;
  QString fileName = QFileDialog::getSaveFileName(this,tr("Export"), settings->lastPath(), tr(GRAPHIC_FILE_FORMATS), &filter);

  // Abort if filename is empty
  if (fileName.isEmpty()) return false;

  // Save accessed path
  settings->setLastPath(QFileInfo(fileName).path());

  // Finding the right extension
  if (QFileInfo(fileName).suffix().isEmpty())
  {
    int index = filter.indexOf(QRegularExpression("\\*."));
    filter = filter.remove(0, index + 1);
    index = filter.indexOf(QRegularExpression("( \\*.)|(\\))"));
    if (index > 0) filter.truncate(index);
    fileName = fileName + filter;
  }
  qDebug() << "Trying to export as " << fileName << "\n";

  settings->setLastPath(QFileInfo(fileName).path());

  // Try to export the file
  if (fileName.endsWith(".svg")) return Molsketch::saveToSVG(fileName, m_molView->scene());

  if (!Molsketch::exportFile(fileName,m_molView->scene(), settings->pixelScalingFactor())) {
    QMessageBox::critical(this,tr(PROGRAM_NAME),tr("Error while exporting file"),QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
  return true;
}

void MainWindow::paintSceneOn (QPrinter *printer) {
  Molsketch::printFile(*printer,m_molView->scene());
}

bool MainWindow::print()
{
  QPrintPreviewDialog printPreview;
  connect(&printPreview, SIGNAL(paintRequested(QPrinter * )), this, SLOT(paintSceneOn(QPrinter *)));

  printPreview.exec ();
  return true;
}

void MainWindow::cleanChanged(bool clean) {
  setWindowModified(!clean);
}

void MainWindow::setToolButtonStyle(QAction *styleAction)
{
  if (!styleAction) return;
  QMainWindow::setToolButtonStyle((Qt::ToolButtonStyle) styleAction->data().toInt());
}

void MainWindow::about()
{
  QString version(settings->currentVersion().toString()), versionNick(settings->versionNick());
  QMessageBox::about(this, tr("About"),
                     tr("<h3>About Molsketch</h3>\n"
                        "<h4>Version: %1 -- %2</h4>\n"
                        "<p>Molsketch is a program for drawing molecular structures developed by Harm van Eersel at the "
                        "<a href=\"http://www.tue.nl\">Eindhoven University of Technology</a>.</p>\n"
                        "<p>For more info check <A href=\"http://molsketch.sourceforge.net\">http://molsketch.sourceforge.net</a>.</p>\n"
                        "<p>It is <a href=\"http://www.gnu.org/philosophy/free-sw.html\">free software</a> and available under "
                        "the <a href=\"https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\">GNU GPL version 2</a> or later versions of the GPL.</p>\n"
                        "<p>Special thanks to: <ul>\n"
                        "<li>Prof. Dr. H. Zantema (coach of the initial version)</li>\n"
                        "<li>Davy van der Vaart (tester)</li>\n"
                        "<li>Frans Visscher (tester)</li>\n"
                        "<li>Carsten Niehaus (reviewer)</li>\n"
                        "</ul></p>\n"
                        "<p>Copyright 2007 - 2008, Harm van Eersel</p>\n"
                        "<p>Copyright 2009 Tim Vandermeersch</p>\n"
                        "<p>Maintenance since 12/2014: Hendrik Vennekate</p>"
                        "<h3>Translations</h3>"
                        "<table>"
                        "<tr><td>Greek</td><td> - </td><td>Alexander Ploumistos</td></tr>"
                        "<tr><td>Chinese</td><td> - </td><td>Wensi Vennekate</td></tr>"
                        "</table>").arg(version).arg(versionNick));
}

void MainWindow::showReleaseNotes() {
  ReleaseNotesDialog().exec();
}

void MainWindow::createHelpMenu() {
  menuBar()->addSeparator();
  auto helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addActions(QList<QAction*>{
                         ActionContainer::generateAction("", "", tr("Submit &Bug..."),"", tr("Open the browser with the bug tracker"), this, &MainWindow::submitBug),
                         ActionContainer::generateAction("", "", tr("YouTube channel..."), "", tr("Open the browser with the YouTube channel page"), this, &MainWindow::goToYouTube),
                         ActionContainer::generateAction("help-about", ":icons/help-about.svg", tr("&About..."), "", tr("Show the application's About box"), this, &MainWindow::about),
                         ActionContainer::generateAction("", "", tr("Show release notes..."), "", tr("Show the entire release notes list for all versions of Molsketch"), this, &MainWindow::showReleaseNotes)
                       });

  auto aboutQtAction = ActionContainer::generateAction("", "", tr("About &Qt..."), "", tr("Show the Qt library's About box"), this);
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  helpMenu->addAction(aboutQtAction);
#ifdef THIRD_PARTY_LICENSES
  auto thirdPartyLicensesAction = ActionContainer::generateAction("", "", tr("Third-party licenses..."), "", tr("Show licenses of included libraries"), this);
  auto licenseDialog = new LicenseDialog(this);
  connect(thirdPartyLicensesAction, SIGNAL(triggered()), licenseDialog, SLOT(exec()));
  helpMenu->addAction(thirdPartyLicensesAction);
#endif
}

void MainWindow::createFileMenuAndToolBar() {
  auto newAction = ActionContainer::generateAction("document-new", ":icons/document-new.svg", tr("&New"), tr("Ctrl+N"), tr("Create a new file"), this, &MainWindow::newFile);
  auto openAction = ActionContainer::generateAction("document-open", ":icons/document-open.svg", tr("&Open..."), tr("Ctrl+O"), tr("Open an existing file"), this, &MainWindow::open);
  auto saveAction = ActionContainer::generateAction("document-save", ":icons/document-save.svg", tr("&Save"), tr("Ctrl+S"), tr("Save the document to disk"), this, &MainWindow::save);
  auto importAction = ActionContainer::generateAction("document-import", ":icons/document-import.svg", tr("&Import..."), tr("Ctrl+I"), tr("Insert an existing molecule into the document"), this, &MainWindow::importDoc);
  auto exportAction = ActionContainer::generateAction("document-export", ":icons/document-export.svg", tr("&Export..."), tr("Ctrl+E"), tr("Export the current document as a picture"), this, &MainWindow::exportDoc);
  auto printAction = ActionContainer::generateAction("document-print", ":icons/document-print.svg", tr("&Print..."), tr("Ctrl+P"), tr("Print the current document"), this, &MainWindow::print);

  auto fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addActions(QList<QAction*>{ newAction, openAction, saveAction,
                        ActionContainer::generateAction("document-save-as", ":icons/document-save-as.svg", tr("Save &As..."), tr("Ctrl+Shift+S"), tr("Save the document under a new name"), this, &MainWindow::saveAs)});
  fileMenu->addSeparator();
  fileMenu->addActions({ importAction, exportAction, printAction });
  fileMenu->addSeparator();
  fileMenu->addAction(ActionContainer::generateAction("preferences-system", ":icons/preferences-system.svg", tr("Pre&ferences..."), tr("Ctrl+F"), tr("Edit your preferences"), this, &MainWindow::editPreferences)
        );
  fileMenu->addSeparator();
  fileMenu->addAction(ActionContainer::generateAction("application-exit", ":icons/application-exit.svg", tr("E&xit"), tr("Ctrl+Q"), tr("Exit the application"), this, &MainWindow::close));

  auto fileToolBar = addToolBar(tr("File"));
  fileToolBar->setObjectName("file-toolbar");
  fileToolBar->addAction(newAction);
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(saveAction);
  fileToolBar->addAction(importAction);
  fileToolBar->addAction(exportAction);
  fileToolBar->addAction(printAction);
}

void MainWindow::createStatusBar()
{
#ifdef __ANDROID__
  QFont statusFont(statusBar()->font());
  statusFont.setPixelSize(12);
  statusBar()->setFont(statusFont);
#endif

  Indicator *openBabelIndicator = new Indicator(tr("OpenBabel"), statusBar());
  openBabelIndicator->setToolTip(tr("Indicates if OpenBabel was found and could be loaded.\n"
                                    "Can be installed using the installer."));
  Indicator *inchiIndicator = new Indicator(tr("InChI"), statusBar());
  inchiIndicator->setToolTip(tr("Indicates if InChI format support is available.\n"
                                "Can be installed using the installer."));

  Indicator *gen2dIndicator = new Indicator(tr("gen2d"), statusBar());
  gen2dIndicator->setToolTip(tr("Indicates if 'gen2d' of OpenBabel is available.\n"
                                "Required for structure optimization."));

  connect(settings, SIGNAL(changedObabelIfacePath(QString)), obabelLoader, SLOT(reloadObabelIface(QString)));
  connect(settings, SIGNAL(changeObabelFormatsPath(QString)), obabelLoader, SLOT(setObabelFormats(QString)));
  connect(obabelLoader, SIGNAL(obabelIfaceAvailable(bool)), openBabelIndicator, SLOT(setMode(bool)));
  connect(obabelLoader, SIGNAL(inchiAvailable(bool)), inchiIndicator, SLOT(setMode(bool)));
  connect(obabelLoader, SIGNAL(optimizeAvailable(bool)), gen2dIndicator, SLOT(setMode(bool)));
  statusBar()->addPermanentWidget(openBabelIndicator);
  statusBar()->addPermanentWidget(inchiIndicator);
  statusBar()->addPermanentWidget(gen2dIndicator);
  auto sumFormulaIndicator = new SumFormulaIndicator(m_molView->scene(), this);
  connect(m_molView->scene(), &MolScene::selectionChanged, sumFormulaIndicator, &SumFormulaIndicator::updateSumFormula);
  statusBar()->addWidget(sumFormulaIndicator);
  obabelLoader->reloadObabelIface(settings->obabelIfacePath());
  obabelLoader->setObabelFormats(settings->obabelFormatsPath());
}

void MainWindow::createToolBarContextMenuOptions()
{
  toolBarTextsAndIcons = new QActionGroup(this);
  toolBarTextsAndIcons->addAction(tr("Icons"))->setData(Qt::ToolButtonIconOnly);
  toolBarTextsAndIcons->addAction(tr("Texts"))->setData(Qt::ToolButtonTextOnly);
  toolBarTextsAndIcons->addAction(tr("Texts under icons"))->setData(Qt::ToolButtonTextUnderIcon);
  toolBarTextsAndIcons->addAction(tr("Texts besides icons"))->setData(Qt::ToolButtonTextBesideIcon);
  toolBarTextsAndIcons->addAction(tr("System default"))->setData(Qt::ToolButtonFollowStyle);
  QMainWindow::setToolButtonStyle(settings->getToolButtonStyle());
  for(QAction* action : toolBarTextsAndIcons->actions())
  {
    action->setCheckable(true);
    if (toolButtonStyle() == action->data())
      action->setChecked(true);
  }
  connect(toolBarTextsAndIcons, SIGNAL(triggered(QAction*)), this, SLOT(setToolButtonStyle(QAction*)));
}

void MainWindow::readSettings()
{
  resize(settings->getWindowSize());
  restoreState(settings->windowState());
  readPreferences();
}

void MainWindow::readPreferences() {
  m_autoSaveTimer->setInterval(settings->autoSaveInterval());
  m_autoSaveTimer->start();
  m_molView->scene()->update();
}

void MainWindow::saveWindowProperties()
{
  settings->setWindowSize(size());
  settings->setToolButtonStyle(toolButtonStyle());
  settings->setWindowState(saveState());
}


bool MainWindow::maybeSave() {
  if (!isWindowModified()) return true;

  QMessageBox::StandardButton ret =
      QMessageBox::warning(this,tr(PROGRAM_NAME),
                           tr("This document has been modified.\n"
                              "Do you want to save your changes?"),
                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  switch (ret) {
    case QMessageBox::Save: return save();
    case QMessageBox::Discard: return true;
    case QMessageBox::Cancel:
    default: return false;
  }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
  // Synthesizing the correct name
  setWindowFilePath(fileName);
  QString shownName;
  if (fileName.isEmpty())
    shownName = tr(EMPTY_FILENAME);
  else
    shownName = QFileInfo(fileName).fileName();

  // Setting the windowtitle
  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr(PROGRAM_NAME))); // TODO use windowFilePath instead
}

void MainWindow::editPreferences()
{
  SettingsDialog dialog(settings);
  connect(&dialog, SIGNAL(settingsChanged()), this, SLOT(readPreferences()));
  dialog.exec();
}

void MainWindow::submitBug() {
  QDesktopServices::openUrl(QUrl("http://sourceforge.net/tracker/?func=add&group_id=191562&atid=937880"));
}

void MainWindow::goToYouTube() {
  QDesktopServices::openUrl(QUrl("https://www.youtube.com/channel/UCIYnNzSnI9AHpB_c48BR7fQ"));
}

