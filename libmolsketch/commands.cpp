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

#include <QTransform>
#include <QDebug>

#include "commands.h"

#include "atom.h"
#include "bond.h"
#include "molecule.h"
#include "molscene.h"
#include "graphicsitem.h"

using Molsketch::Atom;
using Molsketch::Bond;
using Molsketch::Molecule;
using namespace Molsketch::Commands;

ItemAction::ItemAction(QGraphicsItem* newItem, MolScene* addScene, const QString & text)
  : ItemCommand(newItem, text), m_scene(addScene), owning(!newItem->scene()),
    parent(newItem->parentItem())
{}

ItemAction::~ItemAction() {
  if (owning) delete getItem();
}

void ItemAction::redo() {
  if (!getItem() || !m_scene) return;
  auto item = getItem();
  if (owning) {
    m_scene->addItem(item);
    item->setParentItem(parent);
  } else {
    parent = item->parentItem();
    m_scene->removeItem(item);
  }
  owning = !owning;
}

void ItemAction::undo() {
  redo();
}

void ItemAction::addItemToScene(QGraphicsItem *item, Molsketch::MolScene *scene, const QString& text) {
  if (item->scene()) item->scene()->removeItem(item);
  (new ItemAction(item, scene, text))->execute();
}

void ItemAction::removeItemFromScene(QGraphicsItem *item, const QString &text) {
  (new ItemAction(item, dynamic_cast<MolScene*>(item->scene()), text))->execute();
}

Molsketch::MolScene *Molsketch::Commands::ItemAction::getScene() const {
  return m_scene;
}

MoveItem::MoveItem(QGraphicsItem* item, const QPointF & pos, const QString & text, QUndoCommand* parent)
  : setItemPropertiesCommand(item, pos, text, parent) {}

void MoveItem::redo() {
  setItemPropertiesCommand::redo();
  if (getItem()->type()==Atom::Type && dynamic_cast<Atom*>(getItem())->molecule())
    dynamic_cast<Atom*>(getItem())->molecule()->rebuild();
}

MoveItem *MoveItem::relative(QGraphicsItem *item, const QPointF &shift, const QString &text)
{
  return absolute(item, item->pos() + shift, text);
}

MoveItem *MoveItem::absolute(QGraphicsItem *item, const QPointF &newPos, const QString &text)
// TODO this doesn't seem to work right with Molecule
{
  if (!item) return nullptr;
  return new MoveItem(item, newPos, text);
}

ChildItemCommand::ChildItemCommand(QGraphicsItem *parent, QGraphicsItem *child, const QString &text)
  : ItemCommand(parent, text),
    child(child),
    owning(child && child->parentItem() != parent)
{}

ChildItemCommand::~ChildItemCommand() {
  if (owning) delete child;
}

void ChildItemCommand::undo() {
  redo();
}

void ChildItemCommand::redo() {
  qInfo() << "performing child item command. Owning: " << owning << "child:" << child << "parent:" << getItem();
  if (!child || !getItem()) return;
  if (!owning) {
    child->setParentItem(nullptr);
    if (child->scene()) child->scene()->removeItem(child);
  } else child->setParentItem(getItem());
  owning = !owning;
}


void ToggleScene::redo() {
  if (getItem()->scene()) {
    getItem()->scene()->removeItem(getItem());
    owning = true;
  } else {
    otherScene->addItem(getItem());
    owning = false;
  }
}

Molsketch::MolScene *Molsketch::Commands::ToggleScene::getScene() const {
  if (otherScene) return qobject_cast<MolScene*>(otherScene);
  return qobject_cast<MolScene*>(getItem()->scene());
}

ToggleScene::ToggleScene(QGraphicsItem *item, QGraphicsScene *scene, const QString &text, QUndoCommand *parent)
  : SceneCommand(item, text, parent),
    otherScene(scene),
    owning(!item->scene())
{}

ToggleScene::~ToggleScene() {
  if (owning) delete getItem();
}

void Molsketch::Commands::addItemToMolecule(Molsketch::graphicsItem *item, Molsketch::Molecule *molecule, Molsketch::MolScene *scene, const QString &text) {
  auto command = new QUndoCommand(text);
  new ToggleScene(item, scene, "", command);
  new SetParentItem(item, molecule, "", command);
  scene->stack()->push(command);
}

void Molsketch::Commands::removeItemFromMolecule(Molsketch::graphicsItem *item, Molsketch::MolScene *scene, const QString &text)
{
  auto command = new QUndoCommand(text);
  new SetParentItem(item, nullptr, "", command);
  new ToggleScene(item, scene, "", command);
  scene->stack()->push(command);
}
