/***************************************************************************
 *   Copyright (C) 2007 by Harm van Eersel                                 *
 *   devsciurus@xs4all.nl                                                  *
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
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

/** @file
 * This file is part of molsKetch and contains the command classes used in
 * the Qt Redo/Undo system. Each class represents an action on the scene and
 * contains the code to redo/undo that action.
 *
 * @author Harm van Eersel <devsciurus@xs4all.nl>
 * @since Deuterium
 */


#ifndef commands_H
#define commands_H

#include <QUndoCommand>
#include <QPointF>
#include <functional>

class QGraphicsItem;
class QGraphicsScene;
class QTransform;

#include "bond.h"
#include "arrow.h"
#include "frame.h"
#include "molecule.h"
#include "molscene.h"

namespace Molsketch {

  class Atom;
  class Molecule;
  class MolScene;

  namespace Commands { // TODO make null safe and rework

    enum CommandId {
      BondTypeId = 1,
      ArrowTypeId,
      ArrowPropertiesId,
      FrameTypeId,
      ArrowSplineId,
      AtomChargeId,
      AtomImplicitHydrogensId,
      MoveItemId,
      MoleculeNameId,
      CoordinateId,
      SettingsItemId,
    };

    template<class ItemType, class OwnType, int CommandId = -1>
    class Command : public QUndoCommand {
      ItemType *item;
    protected:
      virtual ItemType* getItem() const { return item; }
      virtual QUndoStack *getStack() = 0;
    public:
      explicit Command(const QString& text = "", QUndoCommand* parent = 0)
        : Command(nullptr, text, parent) {}
      explicit Command(ItemType* item, const QString& text = "", QUndoCommand* parent = 0)
        : QUndoCommand(text, parent), item(item) {}
      void undo() override { redo(); }
      int id() const override { return CommandId; }
      bool mergeWith(const QUndoCommand *other) override {
        auto otherCommand = dynamic_cast<const OwnType*>(other);
        if (!otherCommand) return false;
        if (otherCommand->getItem() != this->getItem()) return false;
        return true;
      }
      void execute() {
        QUndoStack *stack = getStack();
        if (stack) stack->push(this);
        else {
          redo();
          delete this;
        }
      }
    };

    template<class ItemType, class OwnType, int CommandId = -1>
    class SceneCommand : public Command<ItemType, OwnType, CommandId> {
    protected:
      virtual MolScene* getScene() const = 0;
      QUndoStack *getStack() override {
        MolScene *scene = getScene();
        return scene ? scene->stack() : nullptr;
      }
    public:
      explicit SceneCommand(const QString& text = "", QUndoCommand* parent = 0)
        : SceneCommand(nullptr, text, parent) {}
      explicit SceneCommand(ItemType* item, const QString& text = "", QUndoCommand* parent = 0)
        : Command<ItemType, OwnType, CommandId>(item, text, parent) {}
    };

    template<class ItemType, class OwnType, int CommandId = -1>
    class ItemCommand : public SceneCommand<ItemType, OwnType, CommandId> { // TODO unit test
    protected: // TODO make sure that ItemType inherits graphicsItem
      virtual MolScene* getScene() const override { ItemType* actualItem = this->getItem();
                                           return actualItem ? dynamic_cast<MolScene*>(actualItem->scene()) : nullptr; }
    public:
      explicit ItemCommand(const QString& text = "", QUndoCommand* parent = 0)
        : ItemCommand(0, text, parent) {}
      explicit ItemCommand(ItemType* item, const QString& text = "", QUndoCommand* parent = 0)
        : SceneCommand<ItemType, OwnType, CommandId>(item, text, parent) {}
    };

    template<class ItemClass,
             class ItemPropertyType,
             void (ItemClass::*setFunction)(const ItemPropertyType&),
             ItemPropertyType (ItemClass::*getFunction)()const,
             int CommandId = -1>
    class setItemPropertiesCommand : public ItemCommand<ItemClass, setItemPropertiesCommand<ItemClass, ItemPropertyType, setFunction, getFunction, CommandId>, CommandId>
    {
    private:
      ItemPropertyType type;
      typedef setItemPropertiesCommand<ItemClass, ItemPropertyType, setFunction, getFunction, CommandId> ownType;
    public:
      setItemPropertiesCommand(ItemClass *item, ItemPropertyType newType, const QString& text = "", QUndoCommand *parent = 0)
        : ItemCommand<ItemClass, ownType, CommandId>(item, text, parent),
          type(newType){}
      void redo() override {
        ItemPropertyType temp = (this->getItem()->*getFunction)();
        (this->getItem()->*setFunction)(type);
        type = temp;
        this->getItem()->update();
      }
    };

    template<class ItemClass,
             class ItemProperty,
             void (ItemClass::*setter)(ItemProperty),
             ItemProperty (ItemClass::*getter)() const,
             int CommandId = -1>
    class SetItemProperty : public ItemCommand<ItemClass, SetItemProperty<ItemClass, ItemProperty, setter, getter, CommandId>, CommandId >
    {
    private:
      ItemProperty value;
      typedef SetItemProperty<ItemClass, ItemProperty, setter, getter, CommandId> ownType;
    public:
      SetItemProperty(ItemClass *item, ItemProperty value, const QString& text = "", QUndoCommand *parent = 0)
        : ItemCommand<ItemClass, ownType, CommandId>(item, text, parent), value(value) {}
      void redo() override {
        ItemProperty temp = (this->getItem()->*getter)();
        (this->getItem()->*setter)(value);
        value = temp;
        this->getItem()->update();
      }
    };

    // TODO replace with SetParent
    class ChildItemCommand : public ItemCommand<QGraphicsItem, ChildItemCommand> {
    public:
      ChildItemCommand(QGraphicsItem* parent, QGraphicsItem* child, const QString& text = "");
      virtual ~ChildItemCommand();
    protected:
      void undo() override;
      void redo() override;
    private:
      QGraphicsItem *child;
      bool owning;
    };

    typedef setItemPropertiesCommand<Bond, Bond::BondType, &Bond::setType, &Bond::bondType, BondTypeId> SetBondType;
    typedef setItemPropertiesCommand<Arrow, Arrow::ArrowType, &Arrow::setArrowType, &Arrow::getArrowType, ArrowTypeId> SetArrowType;
    typedef setItemPropertiesCommand<Arrow, Arrow::Properties, &Arrow::setProperties, &Arrow::getProperties, ArrowPropertiesId> setArrowProperties;
    typedef setItemPropertiesCommand<Frame, QString, &Frame::setFrameString, &Frame::frameString, FrameTypeId> SetFrameTypeString;
    typedef setItemPropertiesCommand<Arrow, bool, &Arrow::setSpline, &Arrow::getSpline, ArrowSplineId> setArrowSplineCommand;
    typedef setItemPropertiesCommand<Atom, int, &Atom::setCharge, &Atom::charge, AtomChargeId> setAtomChargeCommand;
    typedef setItemPropertiesCommand<Atom, quint8, &Atom::setNumImplicitHydrogens, &Atom::numImplicitHydrogens, AtomImplicitHydrogensId> setImplicitHydrogensCommand;
    typedef setItemPropertiesCommand<Atom, QString, &Atom::setElement, &Atom::element> ChangeElement;
    typedef setItemPropertiesCommand<Molecule, QString, &Molecule::setName, &Molecule::getName, MoleculeNameId> ChangeMoleculeName;
    typedef setItemPropertiesCommand<graphicsItem, qreal, &graphicsItem::setRelativeWidth, &graphicsItem::relativeWidth> changeRelativeWidth;
    typedef SetItemProperty<QGraphicsItem, qreal, &QGraphicsItem::setZValue, & QGraphicsItem::zValue> SetZValue;
    typedef setItemPropertiesCommand<graphicsItem, QColor, &graphicsItem::setColor, &graphicsItem::getColor> changeColor;
    typedef setItemPropertiesCommand<Atom, qreal, &Atom::setNewmanDiameter, &Atom::getNewmanDiameter> SetNewmanDiameter;
    typedef setItemPropertiesCommand<Atom, Atom::ShapeType, &Atom::setShapeType, &Atom::shapeType> SetShapeType;
    typedef setItemPropertiesCommand<Atom, NeighborAlignment, &Atom::setHAlignment, &Atom::hAlignment> SetHAlignment;
    typedef setItemPropertiesCommand<graphicsItem, QPolygonF, &graphicsItem::setCoordinates, &graphicsItem::coordinates, CoordinateId> SetCoordinateCommand;
    typedef SetItemProperty<QGraphicsItem, QGraphicsItem*, &QGraphicsItem::setParentItem, &QGraphicsItem::parentItem> SetParentItem;

    class ToggleScene : public SceneCommand<QGraphicsItem, ToggleScene> {
    private:
      void redo() override;
      MolScene *getScene() const override;
      QGraphicsScene *otherScene;
      bool owning;
    public:
      ToggleScene(QGraphicsItem *item, QGraphicsScene *scene, const QString &text = "", QUndoCommand *parent = 0);
      ~ToggleScene();
    };

    class SwapBondAtoms : public setItemPropertiesCommand<Bond, QPair<Atom*, Atom*>, &Bond::setAtoms, &Bond::atoms> {
    public:
      SwapBondAtoms(Bond *bond, const QString &text = "", QUndoCommand* parent = 0)
        : setItemPropertiesCommand(bond, qMakePair(bond->endAtom(), bond->beginAtom()), text, parent ) {}
    };

    class ItemAction : public ItemCommand<QGraphicsItem, ItemAction> {
      ItemAction(QGraphicsItem* newItem, MolScene* addScene, const QString & text = "");
    private:
      ~ItemAction();
      virtual void undo() override;
      virtual void redo() override;
    public:
      static void addItemToScene(QGraphicsItem* item, MolScene* scene, const QString &text = "");
      static void removeItemFromScene(QGraphicsItem* item, const QString &text = "");
    private:
      MolScene *m_scene;
      bool owning;
      QGraphicsItem *parent;
    protected:
      MolScene *getScene() const override;
    };

    class MoveItem : public setItemPropertiesCommand<QGraphicsItem, QPointF, &QGraphicsItem::setPos, &QGraphicsItem::pos, MoveItemId>
    {
    public:
      virtual void redo() override;
      static MoveItem* relative(QGraphicsItem* item, const QPointF& shift, const QString& text = "");
      static MoveItem* absolute(QGraphicsItem* item, const QPointF& newPos, const QString& text = "");
    private:
      MoveItem(QGraphicsItem* item, const QPointF& newPosition, const QString& text = "", QUndoCommand *parent = 0);
    };

    void addItemToMolecule(graphicsItem *item, Molecule *molecule, MolScene *scene, const QString &text = "");
    void removeItemFromMolecule(graphicsItem *item, MolScene *scene, const QString &text="");

  } // namespace Commands

} // namespace Molsketch

#endif
