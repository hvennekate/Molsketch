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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "librarylistwidget.h"

#include <QMainWindow>
#include <QPrinter>

class QAction;
class QMenu;
class QToolBox;
class QDockWidget;
class QTextEdit;
class QUndoStack;
class QListWidget;
class QListWidgetItem;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class OBabelIfaceLoader;
class QProcess ;
class QTimer;
class QActionGroup;
class ApplicationSettings;
class ActionContainer;

namespace Molsketch {
  class Molecule;
  class MolScene;
  class MolView;
}

/**
 * The main application window of molsKetch.
 *
 * @author Harm van Eersel
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

  QMenu* createPopupMenu() override;

  void openFile(const QString &fileName);
protected:
  /** Reimplements the close event to asked for a save on exit. */
  void closeEvent(QCloseEvent *event) override;

  explicit MainWindow(ApplicationSettings* appSetttings);

private slots:
  /** Opens a new empty file. */
  void newFile();
  /** Opens a Open file dialog. */
  void open();
  /** Saves the current document. */
  bool save();
  /** Saves the current document under a new name. */
  bool saveAs();
  /** Saves a backup of the current document. */
  bool autoSave();
  /** Import a file in the current document. */
  bool importDoc();
  /** Export the current document as a picture. */
  bool exportDoc();
  /** Prints the current document. */
  bool print();

  void cleanChanged(bool clean);

  void setToolButtonStyle(QAction* styleAction);

        //used for print preview
        void paintSceneOn (QPrinter *);

  /** Open the preferences editor. */
  void editPreferences();

  void submitBug();
  void goToYouTube();
  /** Open the molsKetch about dialog. */
  void about();

  /** Reloads the preferences */
  void readPreferences();
  void showReleaseNotes();
private:
  /** Creates the status bar of the MainWindow. */
  void createStatusBar();
  /** Creates the toolboxes of the MainWindow. */
  void createToolBox();

  void createToolBarContextMenuOptions();

  /** Loads the previous settings of molsKetch. */
  void readSettings();
  /** Saves the current settings of molsKetch. */
  void saveWindowProperties();

  /** Ask if the document should be saved. */
  bool maybeSave();
  /** Clears the scene. */
  void clearScene();

  /** Set the current file name to @p fileName. */
  void setCurrentFile(const QString &fileName);

  // Timers
  /** The timer for the auto-save action */
  QTimer * m_autoSaveTimer;

  Molsketch::MolView* m_molView;

  QActionGroup* toolBarTextsAndIcons;

  ApplicationSettings *settings;
  OBabelIfaceLoader *obabelLoader;
  ActionContainer *actionContainer;

  void readToSceneUsingOpenBabel(const QString &fileName);
  bool saveFile(const QString &filename);
  void createHelpMenu();
  void createFileMenuAndToolBar();
};

QString readFileContent(const QString& absolutePath);

#endif
