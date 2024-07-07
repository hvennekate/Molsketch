/***************************************************************************
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

#include <cxxtest/TestSuite.h>
#include <mainwindow.h>
#include <QDebug>
#include <QMenu>
#include <QTest>
#include <QMenuBar>
#include <QTimer>
#include <QToolButton>
#include <QPushButton>
#include <molview.h>
#include <QDialogButtonBox>
#include "noargsignalcounter.h"
#include "utilities.h"

class MainWindowAcceptanceTest : public CxxTest::TestSuite {
private:
  MainWindow *window;

  int mainWindowCount() {
    auto topLevelWidgets = qApp->topLevelWidgets();
    return std::count_if(topLevelWidgets.begin(), topLevelWidgets.end(),
                         [](QWidget *w) {return dynamic_cast<MainWindow*>(w);});
  }

  QList<QWidget*> collectMainWindows() {
    auto topLevelWidgets = qApp->topLevelWidgets();
    QList<QWidget*> mainWindows;
    std::copy_if(topLevelWidgets.begin(), topLevelWidgets.end(), std::back_inserter(mainWindows),
                   [](QWidget* w){ return dynamic_cast<MainWindow*>(w); });
    return mainWindows;
  }

  MainWindow *activeMainWindow() {
    auto topLevelWidgets = collectMainWindows();
    return topLevelWidgets.empty() ? nullptr : qobject_cast<MainWindow*>(topLevelWidgets.first());
  }

public:
  void setUp() {
    window = new MainWindow;
    window->show();
  }

  void tearDown() {
    while (auto mainWindow = activeMainWindow()) delete mainWindow;
    TS_ASSERT(qApp->topLevelWidgets().empty());
  }

  void testFileNewOpensNewIndependentWindow() {
    clickMenuEntry({"File", "New"}, window);
    TS_ASSERT_EQUALS(mainWindowCount(), 2);
  }

  void testClosingOriginalWindowDoesNotImpactNewWindow() {
    clickMenuEntry({"File", "New"}, window);
    NoArgSignalCounter destroyedSignalCounter;
    QObject::connect(window, &QObject::destroyed, &destroyedSignalCounter, &NoArgSignalCounter::record);
    window->close();
    delete window;
    TS_ASSERT_EQUALS(destroyedSignalCounter.count, 1);
    TS_ASSERT_EQUALS(mainWindowCount(), 1);
  }

  void testFileNewOpensNewWindowThatDoesNotImpactOldOne() {
    clickMenuEntry({"File", "New"}, window);
    auto windows = collectMainWindows();
    windows.removeAll(window);
    qDebug() << windows;
    TS_ASSERT_EQUALS(windows.size(), 1);
    auto otherWindow = windows.at(0);
    NoArgSignalCounter destroyedSignalCounter;
    QObject::connect(otherWindow, &QObject::destroyed, &destroyedSignalCounter, &NoArgSignalCounter::record);
    otherWindow->close();
    delete otherWindow;
    TS_ASSERT_EQUALS(mainWindowCount(), 1);
    TS_ASSERT_EQUALS(destroyedSignalCounter.count, 1);
  }

  void testWindowModificationStatus() {
    QAction *drawAction = assertNotNull(window->findChild<QAction*>("draw-action"));
    QToolButton *drawButton = assertNotNull(findFirstByType<QToolButton>(drawAction->associatedWidgets()));
    leftMouseClick(drawButton);
    leftMouseClick(window->findChild<Molsketch::MolView*>()->viewport());

    bool saveDialogShown = false;
    QTimer::singleShot(0, [&] {
      saveDialogShown = true;
      QWidget *dialog = assertNotNull(QApplication::activeModalWidget());
      QPushButton *discardButton = assertNotNull(dialog->findChild<QDialogButtonBox*>())->button(QDialogButtonBox::Discard);
      assertNotNull(discardButton);
      leftMouseClick(discardButton);
    });
    window->close();
    TS_ASSERT(saveDialogShown);
  }
};
