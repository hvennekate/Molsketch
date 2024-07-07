/***************************************************************************
 *   Copyright (C) 2016 by Hendrik Vennekate                               *
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

#include <QSet>
#include <QUndoCommand>
#include <molscene.h>
#include <propertieswidget.h>
#include <cxxtest/TestSuite.h>
#include <functional>

using namespace Molsketch;

// TODO nest in test class?
class PropertiesWidgetForTesting : public PropertiesWidget {
  void propertiesChanged() { ++ timesPropertyChanged; }
public:
  int timesPropertyChanged;
  PropertiesWidgetForTesting() : timesPropertyChanged(0) {}
  void push(QUndoCommand* cmd) { attemptToPushUndoCommand(cmd); }
  MolScene* getScene() { return scene(); }
};

class PropertiesWidgetForBlockingTest : public PropertiesWidget{
  void propertiesChanged() { blockedDuringPropertiesChange = blocked(); }
public:
  bool blockedDuringPropertiesChange;
  PropertiesWidgetForBlockingTest() : blockedDuringPropertiesChange(false) {}
  bool isBlocked() { return blocked(); }
  void push(QUndoCommand* cmd) { attemptToPushUndoCommand(cmd); }
};

class WithCallback {
protected:
  std::function<void()> callback;
  WithCallback() : callback([]{}) {}
public:
  void setCallback(std::function<void()> callback) { this->callback = callback; }
};

class WithDestructorCallback {
protected:
  std::function<void()> destructorCallback;
  WithDestructorCallback() : destructorCallback([]{}) {}
public:
  void setDestructorCallback(std::function<void()> callback) { this->destructorCallback = callback; }
  virtual ~WithDestructorCallback() { destructorCallback(); }
  void resetDestructorCallback() { destructorCallback = []{}; }
};

class PropertiesWidgetWithChangeCallback
    : public PropertiesWidget, public WithCallback, public WithDestructorCallback {
  void propertiesChanged() { callback(); }
public:
  void push(QUndoCommand *cmd) { attemptToPushUndoCommand(cmd); }
  MolScene* getScene() { return scene(); }
};

class UndoCommandWithCallback : public QUndoCommand, public WithCallback, public WithDestructorCallback {
  void redo() { callback(); }
};

class UndoCommandForTesting : public QUndoCommand {
public:
  static int undone, redone, destroyed;
  UndoCommandForTesting() { resetCounters(); }
  void redo() { ++redone; }
  void undo() { ++undone; }
  ~UndoCommandForTesting() { ++destroyed; }
  static void resetCounters() { undone = 0; redone = 0; destroyed = 0; }
};

int UndoCommandForTesting::undone,
UndoCommandForTesting::redone,
UndoCommandForTesting::destroyed;

class UndoCommandForRecursion : public QUndoCommand {
  PropertiesWidgetForBlockingTest * widget;
public:
  UndoCommandForRecursion(PropertiesWidgetForBlockingTest *widget)
    : widget(widget) { blockedDuringExecution = false; }
  static bool blockedDuringExecution;
  void redo() { blockedDuringExecution = widget->isBlocked(); }
};

bool UndoCommandForRecursion::blockedDuringExecution;

class MolSceneForTesting : public MolScene {
public:
  int receiverCount(const char *signal) const { return receivers(signal); }
};

class GraphicsItemForTesting : public graphicsItem {
public:
  QString xmlName() const override { return ""; }
  QRectF boundingRect() const override { return QRectF(); }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
  { Q_UNUSED(painter); Q_UNUSED(option); Q_UNUSED(widget)}
  QPolygonF coordinates() const override { return QPolygonF(); }
  void setCoordinates(const QVector<QPointF> &c) override {Q_UNUSED(c)}
};


// propertyWidget is connected to undoStack by mapper or scene
//  - connected to scene:
//    - connects/disconnects to undoStack
//    - connects/disconnects to selection change
//    - connection -> mapper
// Two services are provided:
//  - call purely virtual function upon undoStack change
//    (- argument: list of selected items)
//later:- condition: there are selected items
//     - number is matched
//     - type is matched
//later:-> use precondition/matcher callback
//  - push command to undoStack
//    - provide list of selected items
//    - locked
// when parent changed -> check if new parent is scene
class PropertyWidgetUnitTest : public CxxTest::TestSuite
{
private:
  PropertiesWidgetForTesting* widget;
  PropertiesWidgetForBlockingTest *blockingWidget;
  PropertiesWidgetWithChangeCallback *callbackWidget;

public:

  void setUp() {
    widget = new PropertiesWidgetForTesting;
    blockingWidget = new PropertiesWidgetForBlockingTest;
    callbackWidget = new PropertiesWidgetWithChangeCallback;
  }

  void tearDown() {
    TS_ASSERT(!blockingWidget->isBlocked());
    delete widget;
    delete blockingWidget;
    delete callbackWidget;
  }

  void testReactionToPropertyChangeSlot() {
    int timesPropertyChanged = 0;
    callbackWidget->setCallback( [&]() { ++timesPropertyChanged; } );
    callbackWidget->propertiesChange();
    TS_ASSERT_EQUALS(timesPropertyChanged, 1);
  }

  void testPropertyChangeSlotConnectedToUndoStack() {
    MolSceneForTesting scene;
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 0);
    widget->setScene(&scene);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 1);
    scene.stack()->push(new UndoCommandForTesting);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 2);
  }

  void testPropertyChangeSlotNotCalledAfterSceneChange() {
    MolSceneForTesting scene;
    widget->setScene(&scene);
    widget->setScene(0);
    widget->timesPropertyChanged = 0;
    scene.stack()->push(new UndoCommandForTesting);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 0);
  }


  void testPropertyChangeSlotConnectedToSelectionChanged() {
    MolSceneForTesting scene;
    widget->setScene(&scene);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 1);
    GraphicsItemForTesting *item = new GraphicsItemForTesting;
    scene.addItem(item);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 1);
    item->setSelected(true);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 2);
  }


  UndoCommandForTesting* assertPushedCommandGetsExecuted() {
    UndoCommandForTesting *command = new UndoCommandForTesting;
    widget->push(command);
    TS_ASSERT_EQUALS(UndoCommandForTesting::redone, 1);
    return command;
  }

  void testUndoCommandIsExecutedWhenAttemptingToPush() {
    assertPushedCommandGetsExecuted();
    TS_ASSERT_EQUALS(UndoCommandForTesting::destroyed, 1);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 0);
  }

  void testSceneCanBeSet() {
    MolSceneForTesting scene;
    int originalDestroyedReceivers = scene.receiverCount(SIGNAL(destroyed(QObject*)));
    int originalSelectionChangedReceivers = scene.receiverCount(SIGNAL(selectionChanged()));
    widget->setScene(&scene);
    TS_ASSERT_EQUALS(widget->getScene(), &scene);
    TS_ASSERT_EQUALS(scene.receiverCount(SIGNAL(destroyed(QObject*))), originalDestroyedReceivers + 1);
    TS_ASSERT_EQUALS(scene.receiverCount(SIGNAL(selectionChanged())), originalSelectionChangedReceivers + 1);
    TS_ASSERT_EQUALS(widget->timesPropertyChanged, 1);
  }

  void testSceneGetsRemovedWhenDeletedAndWidgetDisabled() {
    TS_ASSERT(!widget->isEnabled());
    MolScene *scene = new MolScene();
    widget->setScene(scene);
    TS_ASSERT(widget->isEnabled());
    delete scene;
    TS_ASSERT_EQUALS(widget->getScene(), nullptr);
    TS_ASSERT(!widget->isEnabled());
  }

  void testSignalsGetRemovedWhenChangingScene() {
    MolSceneForTesting scene;
    widget->setScene(&scene);
    int originalDestroyedReceivers = scene.receiverCount(SIGNAL(destroyed(QObject*)));
    int originalSelectionChangedReceivers = scene.receiverCount(SIGNAL(selectionChanged()));
    widget->setScene(0);
    TS_ASSERT_EQUALS(scene.receiverCount(SIGNAL(destroyed(QObject*))), originalDestroyedReceivers - 1);
    TS_ASSERT_EQUALS(scene.receiverCount(SIGNAL(selectionChanged())), originalSelectionChangedReceivers - 1);
  }

  void testUndoCommandGetsPushedIfStackIsAvailable() {
    MolScene scene;
    widget->setScene(&scene);
    QUndoCommand* command = assertPushedCommandGetsExecuted();
    TS_ASSERT_EQUALS(UndoCommandForTesting::destroyed, 0);
    TS_ASSERT_EQUALS(scene.stack()->count(), 1);
    TS_ASSERT_EQUALS(scene.stack()->command(0), command);
  }

  void testIfSceneGetsDeletedUndoCommandGetsExecutedAndDeleted() {
    MolScene *scene = new MolScene;
    widget->setScene(scene);
    delete scene;
    assertPushedCommandGetsExecuted();
    TS_ASSERT_EQUALS(UndoCommandForTesting::destroyed, 1);
  }

  void testDuringPropertiesChangeBlockedIsTrue() {
    TS_ASSERT(!blockingWidget->blockedDuringPropertiesChange);
    blockingWidget->propertiesChange();
    TS_ASSERT(blockingWidget->blockedDuringPropertiesChange);
  }

  void testDuringUndoPushBlockedIsTrue() {
    UndoCommandForRecursion *cmd = new UndoCommandForRecursion(blockingWidget);
    TS_ASSERT(!UndoCommandForRecursion::blockedDuringExecution);
    blockingWidget->push(cmd);
    TS_ASSERT(UndoCommandForRecursion::blockedDuringExecution);
  }

  void testDuringUndoPushBlockedIsTrueWithScene() {
    UndoCommandForRecursion *cmd = new UndoCommandForRecursion(blockingWidget);
    MolSceneForTesting scene;
    blockingWidget->setScene(&scene);
    TS_ASSERT(!UndoCommandForRecursion::blockedDuringExecution);
    blockingWidget->push(cmd);
    TS_ASSERT(UndoCommandForRecursion::blockedDuringExecution);
  }

  void testDuringPropertiesChangePropertiesChangeIsBlocked() {
    bool recursivelyCalled = false;
    bool propertyChangeCalled = false;
    PropertiesWidgetWithChangeCallback callbackWidget;
    callbackWidget.setCallback([&] {
      if (propertyChangeCalled) {
        recursivelyCalled = true;
        return;
      }
      propertyChangeCalled = true;
      callbackWidget.propertiesChange();
    });
    callbackWidget.propertiesChange();
    TS_ASSERT(propertyChangeCalled);
    TS_ASSERT(!recursivelyCalled);
  }

  void testDuringPropertiesChangeAttemptUndoPushIsBlocked() {
    assertUndoCommandIsNotCalledDuringPropertiesChange();
  }

  void assertUndoCommandIsNotCalledDuringPropertiesChange() {
    UndoCommandWithCallback *undoCommand = new UndoCommandWithCallback;
    bool undoCommandExecuted = false;
    undoCommand->setCallback([&]{ undoCommandExecuted = true; });
    bool undoCommandDestroyed = false;
    undoCommand->setDestructorCallback([&]{ undoCommandDestroyed = true; });

    bool pushing = false;
    callbackWidget->setCallback([&] {
      if (pushing) return;
      pushing = true;
      callbackWidget->push(undoCommand);
    });

    callbackWidget->propertiesChange();

    TS_ASSERT(!undoCommandExecuted);
    TS_ASSERT(undoCommandDestroyed);
    if (!undoCommandDestroyed) undoCommand->resetDestructorCallback();
  }

  void testDuringPropertiesChangeAttemptUndoPushIsBlockedWithScene() {
    MolSceneForTesting scene;
    callbackWidget->setScene(&scene);
    TS_ASSERT_EQUALS(callbackWidget->getScene(), &scene); // TODO put into object
    assertPushedCommandGetsExecuted();
  }

  void testDuringAttemptUndoPushChangePropertiesIsBlocked() {
    assertPropertiesChangedNotCalledDuringUndoPush(0);
  }

  void assertPropertiesChangedNotCalledDuringUndoPush(MolScene *scene)
  {
    UndoCommandWithCallback *undoCommand = new UndoCommandWithCallback;
    undoCommand->setCallback([&]{ callbackWidget->propertiesChange(); });
    bool undoCommandDestroyed = false;
    undoCommand->setDestructorCallback([&]{ undoCommandDestroyed = true; });
    bool propertiesChangedCalled = false;
    callbackWidget->setCallback([&]{ propertiesChangedCalled = true; });
    callbackWidget->push(undoCommand);
    TS_ASSERT(!propertiesChangedCalled);
    assertCommandOnStackOrDestroyed(scene, undoCommand, undoCommandDestroyed);
  }

  void testDuringAttemptUndoPushChangePropertiesIsBlockedWithScene() {
    TS_SKIP("Warning: re-enable! This is causing mysterious segfaults");
    MolSceneForTesting scene;
    callbackWidget->setScene(&scene);
    assertPropertiesChangedNotCalledDuringUndoPush(&scene);
  }

  void testDuringAttemptUndoPushAttemptUndoPushIsBlocked() {
    assertUndoPushIsBlockedDuringUndoPush(0);
  }

  void assertUndoPushIsBlockedDuringUndoPush(MolScene* scene) {
    UndoCommandWithCallback *outerCommand = new UndoCommandWithCallback;
    bool outerCommandDestroyed = false;
    outerCommand->setDestructorCallback([&] { outerCommandDestroyed = true; });

    UndoCommandWithCallback *innerCommand = new UndoCommandWithCallback;
    bool innerCommandDestroyed = false;
    innerCommand->setDestructorCallback([&] {innerCommandDestroyed = true; });

    outerCommand->setCallback([&] { callbackWidget->push(innerCommand); });
    bool innerCommandCalled = false;
    innerCommand->setCallback([&]{ innerCommandCalled = true; });

    callbackWidget->push(outerCommand);

    TS_ASSERT(!innerCommandCalled);
    TS_ASSERT(innerCommandDestroyed);
    if (!innerCommandDestroyed) innerCommand->resetDestructorCallback();
    assertCommandOnStackOrDestroyed(scene, outerCommand, outerCommandDestroyed);
  }

  void assertCommandOnStackOrDestroyed(MolScene *scene, UndoCommandWithCallback *cmd, const bool& wasDestroyed) {
    if (scene) {
     TS_ASSERT_EQUALS(scene->stack()->count(), 1);
     TS_ASSERT_EQUALS(scene->stack()->command(0), cmd);
     if (scene->stack()->command(0) != cmd) delete cmd;
     else cmd->resetDestructorCallback();
    } else {
      TS_ASSERT(wasDestroyed);
      if (!wasDestroyed) delete cmd;
    }
  }

  void testDuringAttemptUndoPushAttemptUndoPushIsBlockedWithScene() {
    MolSceneForTesting scene;
    callbackWidget->setScene(&scene);
    assertUndoPushIsBlockedDuringUndoPush(&scene);
  }

  void testSequentialPushingOfTwoCommands() {
    MolSceneForTesting scene;
    callbackWidget->setScene(&scene);

    UndoCommandWithCallback *outerCommand = new UndoCommandWithCallback;
    bool outerCommandDestroyed = false;
    outerCommand->setDestructorCallback([&] { outerCommandDestroyed = true; });
    bool outerCommandCalled = false;
    outerCommand->setCallback([&] { outerCommandCalled = true; });

    UndoCommandWithCallback *innerCommand = new UndoCommandWithCallback;
    bool innerCommandDestroyed = false;
    innerCommand->setDestructorCallback([&] {innerCommandDestroyed = true; });
    bool innerCommandCalled = false;
    innerCommand->setCallback([&]{ innerCommandCalled = true; });

    callbackWidget->push(outerCommand);
    callbackWidget->push(innerCommand);

    TS_ASSERT(outerCommandCalled);
    TS_ASSERT(!outerCommandDestroyed);
    if (!outerCommandDestroyed) outerCommand->resetDestructorCallback();
    TS_ASSERT(innerCommandCalled);
    TS_ASSERT(!innerCommandDestroyed);
    if (!innerCommandDestroyed) innerCommand->resetDestructorCallback();
  }

};
