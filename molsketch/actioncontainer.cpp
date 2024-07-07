/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2018 (updated) by Hendrik Vennekate                     *
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
#include "actioncontainer.h"
#include "obabelifaceloader.h"
#include "optimizestructureaction.h"

#include <QApplication>
#include <QMenu>
#include <QToolBar>
#include <QClipboard>
#include <molscene.h>
#include <molview.h>

#include <actions/coloraction.h>
#include <actions/deleteaction.h>
#include <actions/drawaction.h>
#include <actions/flipbondaction.h>
#include <actions/flipstereobondsaction.h>
#include <actions/frameaction.h>
#include <actions/incdecaction.h>
#include <actions/itemtypeselectionaction.h>
#include <actions/linewidthaction.h>
#include <actions/mechanismarrowaction.h>
#include <actions/reactionarrowaction.h>
#include <actions/ringaction.h>
#include <actions/rotateaction.h>
#include <actions/textaction.h>
#include <actions/alignmentaction.h>
#include <actions/arrowtypeaction.h>
#include <actions/bondtypeaction.h>
#include <actions/flipbondaction.h>
#include <actions/frametypeaction.h>
#include <actions/zlevelaction.h>
#include <actions/lineupaction.h>
#include <constants.h>

struct ActionContainerPrivate {
  Q_DISABLE_COPY(ActionContainerPrivate)
  QAction *zoomInAction, *zoomOutAction, *zoomResetAction, *zoomFitAction;
  QAction *undoAction, *redoAction, *cutAction, *copyAction, *pasteAction, *selectAllAction, *alignAction;
  Molsketch::MolView *view;
#ifdef QT_DEBUG
  QAction *debugAction;
#endif

  void createZoomActions(Molsketch::MolView *view) {
    zoomInAction = new QAction(QIcon::fromTheme("zoom-in", QIcon(":icons/zoom-in.svg")),QObject::tr("Zoom &In"), view);
    zoomInAction->setShortcut(QObject::tr("Ctrl++"));
    zoomInAction->setStatusTip(QObject::tr("Zoom in on the canvas"));
    QObject::connect(zoomInAction, &QAction::triggered, view, &Molsketch::MolView::zoomIn);

    zoomOutAction = new QAction(QIcon::fromTheme("zoom-out", QIcon(":icons/zoom-out.svg")),QObject::tr("Zoom &Out"), view);
    zoomOutAction->setShortcut(QObject::tr("Ctrl+-"));
    zoomOutAction->setStatusTip(QObject::tr("Zoom out on the canvas"));
    QObject::connect(zoomOutAction, &QAction::triggered, view, &Molsketch::MolView::zoomOut);

    zoomResetAction = new QAction(QIcon::fromTheme("zoom-original", QIcon(":icons/zoom-original.svg")),QObject::tr("Zoom &Reset"), view);
    zoomResetAction->setShortcut(QObject::tr("Ctrl+="));
    zoomResetAction->setStatusTip(QObject::tr("Reset the zoom level"));
    QObject::connect(zoomResetAction, &QAction::triggered, view, &Molsketch::MolView::zoomReset);

    zoomFitAction = new QAction(QIcon::fromTheme("zoom-fit-best", QIcon(":icons/zoom-fit-best.svg")),QObject::tr("Zoom &Fit"), view);
    zoomFitAction->setShortcut(QObject::tr("Ctrl+*"));
    zoomFitAction->setStatusTip(QObject::tr("Fit to screen"));
    QObject::connect(zoomFitAction, &QAction::triggered, view, &Molsketch::MolView::zoomFit);
  }

  void createEditActions(Molsketch::MolScene *scene) {
    undoAction = scene->stack()->createUndoAction(scene);
    undoAction->setIcon(QIcon::fromTheme("edit-undo", QIcon(":icons/edit-undo.svg")));
    undoAction->setShortcut(QObject::tr("Ctrl+Z"));
    undoAction->setStatusTip(QObject::tr("Undo the last action"));

    redoAction = scene->stack()->createRedoAction(scene);
    redoAction->setIcon(QIcon::fromTheme("edit-redo", QIcon(":icons/edit-redo.svg")));
    redoAction->setShortcut(QObject::tr("Ctrl+Shift+Z"));
    redoAction->setStatusTip(QObject::tr("Redo the last action"));

    cutAction = new QAction(QIcon::fromTheme("edit-cut", QIcon(":icons/edit-cut.svg")), QObject::tr("Cu&t"), scene);
    cutAction->setShortcut(QObject::tr("Ctrl+X"));
    cutAction->setStatusTip(QObject::tr("Cut the current selection's contents to the clipboard"));
    cutAction->setEnabled(false);
    QObject::connect(cutAction, &QAction::triggered, scene, &Molsketch::MolScene::cut);

    copyAction = new QAction(QIcon::fromTheme("edit-copy", QIcon(":icons/edit-copy.svg")), QObject::tr("&Copy"), scene);
    copyAction->setShortcut(QObject::tr("Ctrl+C"));
    copyAction->setStatusTip(QObject::tr("Copy the current selection's contents to the clipboard"));
    copyAction->setEnabled(false);
    QObject::connect(copyAction, &QAction::triggered, scene, &Molsketch::MolScene::copy);

    pasteAction = new QAction(QIcon::fromTheme("edit-paste", QIcon(":icons/edit-paste.svg")), QObject::tr("&Paste"), scene);
    pasteAction->setShortcut(QObject::tr("Ctrl+V"));
    pasteAction->setStatusTip(QObject::tr("Paste the clipboard's contents into the current selection"));
    pasteAction->setEnabled(false);
    QObject::connect(pasteAction, &QAction::triggered, scene, &Molsketch::MolScene::paste);

    selectAllAction = new QAction(QIcon::fromTheme("edit-select-all", QIcon(":icons/edit-select-all.svg")), QObject::tr("&Select all"), scene);
    selectAllAction->setShortcut(QObject::tr("Ctrl+A"));
    selectAllAction->setStatusTip(QObject::tr("Selects all elements on the scene"));
    QObject::connect(selectAllAction, &QAction::triggered, scene, &Molsketch::MolScene::selectAll);

    alignAction = new QAction(QIcon(""), QObject::tr("Show grid"), scene);
    alignAction->setStatusTip(QObject::tr("Shows grid and snaps to it while drawing"));
    alignAction->setCheckable(true);
    QObject::connect(alignAction, &QAction::toggled, scene, &Molsketch::MolScene::setGrid);
  }

  ActionContainerPrivate(Molsketch::MolView *view) : view(view) {
    using namespace Molsketch;
    createZoomActions(view);
    MolScene *scene = view->scene();
    createEditActions(scene);
    QObject::connect(scene, &MolScene::copyAvailable, copyAction, &QAction::setEnabled);
    QObject::connect(scene, &MolScene::copyAvailable, cutAction, &QAction::setEnabled);
    QObject::connect(scene, &MolScene::pasteAvailable, pasteAction, &QAction::setEnabled);
    scene->clipboardChanged();
  }
};

ActionContainer::ActionContainer(Molsketch::MolView *view, QObject *parent)
  : QObject(parent),
    d_ptr(new ActionContainerPrivate(view))
{
#ifdef QT_DEBUG
  Q_D(ActionContainer);
  d->debugAction = new QAction("debug scene", this);
  connect(d->debugAction, &QAction::triggered, view->scene(), &Molsketch::MolScene::debugScene);
#endif
}

ActionContainer::~ActionContainer() {}

QMenu *createMenu(const QString &title, const QString &objectName, QWidget* parent, const QList<QAction*> &actions) {
  auto result = new QMenu(title, parent);
  result->setObjectName(objectName);
  for (auto action : actions)
    if (action) result->addAction(action);
    else result->addSeparator();
  return result;
}


QMenu *ActionContainer::createEditMenu(QWidget *parent) const {
  Q_D(const ActionContainer);
  return createMenu(QObject::tr("&Edit"), "edit-menu", parent,
                    QList<QAction*>{d->undoAction, d->redoAction, nullptr,
                                    d->cutAction, d->copyAction, d->pasteAction, nullptr,
                                    d->selectAllAction, d->alignAction});
}

QMenu *ActionContainer::createViewMenu(QWidget *parent) const {
  Q_D(const ActionContainer);
  return createMenu(QObject::tr("&View"), "zoom-menu", parent, QList<QAction*>{d->zoomInAction, d->zoomOutAction, d->zoomResetAction, d->zoomFitAction});
}

QToolBar *createToolBar(const QString &title, const QString &objectName, QWidget* parent, const QList<QAction*> &actions) {
  auto result = new QToolBar(title, parent);
  result->setObjectName(objectName);
  for (auto action : actions)
    if (action) result->addAction(action);
    else result->addSeparator();
  return result;
}

QToolBar *ActionContainer::createEditToolBar(QWidget *parent) const {
  Q_D(const ActionContainer);
  return createToolBar(QObject::tr("Edit"), "edit-toolbar", parent, QList<QAction*>{d->undoAction, d->redoAction, nullptr, d->cutAction, d->copyAction, d->pasteAction});
}

QToolBar *ActionContainer::createZoomToolBar(QWidget *parent) const {
  Q_D(const ActionContainer);
  return createToolBar(QObject::tr("Zoom"), "zoom-toolbar", parent, QList<QAction*>{d->zoomInAction, d->zoomOutAction, d->zoomResetAction, d->zoomFitAction
#ifdef QT_DEBUG
                       , d->debugAction
#endif
                       });
}

QToolBar *ActionContainer::createDrawingToolBar(Molsketch::MolScene *scene, QWidget* parent) {
  using namespace Molsketch;
  auto drawToolBar = new QToolBar(QObject::tr("Drawing"), parent);
  drawToolBar->setObjectName("drawing-toolbar");
  drawToolBar->addActions({new drawAction(scene),
                           new ringAction(scene),
                           new reactionArrowAction(scene),
                           new mechanismArrowAction(scene),
                           new FrameAction(scene),
                           new TextAction(scene)});
  return drawToolBar;
}

QToolBar *ActionContainer::createModifyToolBar(Molsketch::MolScene *scene, QWidget *parent) {
  using namespace Molsketch;
  auto modifyToolBar = new QToolBar(QObject::tr("Modify"), parent);
  modifyToolBar->setObjectName("modify-toolbar");
  modifyToolBar->addAction(new ItemTypeSelectionAction(scene));
  modifyToolBar->addAction(new rotateAction(scene));
  modifyToolBar->addAction(new colorAction(scene));
  modifyToolBar->addAction(new lineWidthAction(scene));
  modifyToolBar->addAction(new ZLevelAction(scene));
  modifyToolBar->addAction(new ZLevelStepAction(scene));
  modifyToolBar->addAction(new chargeAction(scene));
  modifyToolBar->addAction(new hydrogenAction(scene));
  modifyToolBar->addAction(new deleteAction(scene));
  modifyToolBar->addAction(new flipBondAction(scene));
  modifyToolBar->addAction(new flipStereoBondsAction(scene));
  return modifyToolBar;
}

QToolBar *ActionContainer::createAlignmentToolBar(Molsketch::MolScene *scene, OBabelIfaceLoader *obabelLoader, QWidget *parent) {
  using namespace Molsketch;
  auto alignmentToolBar = new QToolBar(QObject::tr("Align"), parent);
  alignmentToolBar->setObjectName("alignment-toolbar");
  alignmentToolBar->addAction(AlignmentAction::flushLeft(scene));
  alignmentToolBar->addAction(AlignmentAction::atHorizontalCenter(scene));
  alignmentToolBar->addAction(AlignmentAction::flushRight(scene));
  alignmentToolBar->addAction(AlignmentAction::atTop(scene));
  alignmentToolBar->addAction(AlignmentAction::atVerticalCenter(scene));
  alignmentToolBar->addAction(AlignmentAction::atBottom(scene));
  alignmentToolBar->addAction(LineUpAction::horizontal(scene));
  alignmentToolBar->addAction(LineUpAction::vertical(scene));
  alignmentToolBar->addAction(new OptimizeStructureAction(obabelLoader, scene));
  return alignmentToolBar;

}

void ActionContainer::addContextMenuActions(Molsketch::MolScene *scene) {
  using namespace Molsketch;
  new arrowTypeAction(scene);
  new bondTypeAction(scene);
  new flipBondAction(scene);
  new FrameTypeAction(scene);
}

QAction *ActionContainer::generateAction(const QString &themeIcon, const QString &backupIcon, const QString &text, const QString &shortcut, const QString &statusTip, QObject *parent){
  auto action = new QAction(QIcon::fromTheme(themeIcon, QIcon(backupIcon)), text, parent);
  action->setShortcut(shortcut);
  action->setStatusTip(statusTip);
  return action;
}
