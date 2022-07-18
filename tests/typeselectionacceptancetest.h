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

#include <cxxtest/TestSuite.h>
#include <QGraphicsView>
#include <QToolBar>
#include <QTimer>
#include <molscene.h>
#include <QPushButton>
#include <QCheckBox>
#include <QToolButton>
#include "mainwindow.h"
#include "utilities.h"

class TypeSelectionAcceptanceTest : public CxxTest::TestSuite {
  MainWindow *mainWindow;

  QToolButton* findToolButton(QString toolbarName, QString actionName) {
    QToolBar *toolbar = assertNotNull(mainWindow->findChild<QToolBar*>(toolbarName), "Need toolbar: " + toolbarName);
    QAction *typeSelectionAction = assertNotNull(mainWindow->findChild<QAction*>(actionName), "Need action: " + actionName); // TODO write assertion on widget
    QWidget *actionWidget = assertNotNull(toolbar->widgetForAction(typeSelectionAction), "No widget available for action: " + actionName);
    QToolButton *actionButton = assertNotNull(qobject_cast<QToolButton*>(actionWidget), "Widget for action '" + actionName + "' is not a button.");
    TSM_ASSERT("Action button not enabled!", actionButton->isEnabled());
    return actionButton;
  }

  void clickToolButton(QString toolbarName, QString actionName) {
    QTest::mouseClick(findToolButton(toolbarName, actionName), Qt::LeftButton);
  }

  void assertToolButtonNotSelected(QString toolbarName, QString actionName) {
    TS_ASSERT(!findToolButton(toolbarName, actionName)->isChecked())
  }

  void clickOnView() {
    QGraphicsView *view = mainWindow->findChild<QGraphicsView*>();
    QTest::mouseClick(view->viewport(), Qt::LeftButton);
  }

  void selectEverythingFromView() {
    QTest::keyClick(mainWindow, Qt::Key_Escape);
    QTest::keyClicks(mainWindow, "A", Qt::ControlModifier);
    mainWindow->findChild<Molsketch::MolScene*>()->selectAll();
  }

  class ActivateBondCheckBoxAndConfirmDialog {
    QWidget* findDialog() {
      return assertNotNull(qApp->activeModalWidget(), "No type dialog was opened!");
    }

    void clickBondCheckBox(QWidget* typeDialog) {
      clickCheckBox(assertNotNull(typeDialog->findChild<QCheckBox*>("bonds"), "No bond checkbox found!"));
    }

    QWidget* findOkButton() {
      for(QPushButton *child : findDialog()->findChildren<QPushButton*>())
        if (child->text().endsWith("OK")) return child;
      TSM_ASSERT("Could not find OK button!", false);
      throw nullptr;
    }

  public:
    void operator()() {
      clickBondCheckBox(findDialog());
      QTest::mouseClick(findOkButton(), Qt::LeftButton);
    }
  };

  void assertOnlyBondsAreSelected() {
    QList<QGraphicsItem*> selection = assertNotNull(mainWindow->findChild<Molsketch::MolScene*>(), "No MolScene found!")->selectedItems();
    TSM_ASSERT_EQUALS("Selection should contain only three items for cyclopropane", selection.size(), 3);
    for(QGraphicsItem* selectedItem : selection)
      TSM_ASSERT_EQUALS("Item should be of 'bond' type!", selectedItem->type(), Molsketch::Bond::Type);
  }

public:
  void setUp() {
    mainWindow = new MainWindow;
    mainWindow->show();
  }

  void tearDown() {
    delete mainWindow;
  }

  void testSelection() {
    clickToolButton("drawing-toolbar", "ring-action");
    clickOnView();
    selectEverythingFromView();

    QTimer::singleShot(0, mainWindow, ActivateBondCheckBoxAndConfirmDialog());
    clickToolButton("modify-toolbar", "item-type-selection-action");

    assertOnlyBondsAreSelected();
    assertToolButtonNotSelected("modify-toolbar", "item-type-selection-action");
  }

  void testSelectionFromUnselected() {
    clickToolButton("drawing-toolbar", "ring-action");
    clickOnView();

    QTimer::singleShot(0, mainWindow, ActivateBondCheckBoxAndConfirmDialog());
    clickToolButton("modify-toolbar", "item-type-selection-action");

    assertOnlyBondsAreSelected();
    assertToolButtonNotSelected("modify-toolbar", "item-type-selection-action");
  }

  // TODO test for all actions: icon defined
  // TODO test only types available within the current selection are available
};
