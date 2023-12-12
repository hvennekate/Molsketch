/***************************************************************************
 *   Copyright (C) 2016 Hendrik Vennekate                                  *
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
#include "frametypeaction.h"
#include "frametypewidget.h"
#include "frame.h"
#include "molecule.h"
#include "commands.h"
#include "molscene.h"

namespace Molsketch {

  struct FrameTypeAction::privateData
  {
    FrameTypeAction *action;
    privateData(FrameTypeAction *action) : action(action) {}

    bool itemsAreOnlyFramesAndNotEmpty()
    {
      if (action->items().isEmpty()) return false;
      for(auto item : action->items())
      {
        if (item->type() != Frame::Type) return false;
        if (!dynamic_cast<Frame*>(item)) return false;
      }
      return true;
    }
  };

  FrameTypeAction::FrameTypeAction(MolScene *scene) :
    ItemGroupTypeAction(scene),
    d(new privateData(this))
  {
    setItemTypeWidget(new FrameTypeWidget);
    setText(tr("Decoration type"));
  }

  FrameTypeAction::~FrameTypeAction()
  {
    delete d;
  }

  QVariant FrameTypeAction::defaultData() const
  {
    return QVariant("");
  }

  QSet<graphicsItem *> FrameTypeAction::filterItems(const QList<QGraphicsItem *> &inputItems) const
  {
    QSet<graphicsItem*> result;
    foreach(QGraphicsItem* item, inputItems)
    {
      Frame* frame = dynamic_cast<Frame*>(item);
      if (frame)
      {
        result << frame ;
        continue;
      }
      Molecule* molecule = dynamic_cast<Molecule*>(item);
      if (molecule && molecule->parentItem())
      {
        frame = dynamic_cast<Frame*>(molecule->parentItem());
        if (frame) result << frame;
        else result << molecule;
      }
      else result << molecule;
    }
    result.remove(0);
    return result;
  }

  void FrameTypeAction::getType(int &type, QVariant &data) const
  {
    Q_UNUSED(type)
    if (!d->itemsAreOnlyFramesAndNotEmpty()) return;
    auto frame = dynamic_cast<Frame*>(items().first());
    if (!frame) return; // TODO hm...
    data = frame->frameString();
  }

  // TODO make utility function or lambda and copy_if
  bool isFrame(QGraphicsItem* item)
  {
    return dynamic_cast<Frame*>(item) && item->type() == Frame::Type;
  }

  // TODO make utility function or lambda and copy_if
  QSet<graphicsItem *> onlyTopLevelItems(const QList<graphicsItem *>& inputList)
  {
    QSet<graphicsItem *> resultSet;
    for(auto item : inputList)
    {
      QGraphicsItem* topLevelItem = item;
      while (topLevelItem->parentItem())
        topLevelItem = topLevelItem->parentItem();
      resultSet << dynamic_cast<graphicsItem*>(topLevelItem);
    }
    resultSet.remove(0);
    return resultSet;
  }

  void FrameTypeAction::applyType(int type, const QVariant &data) const
  {
    Q_UNUSED(type)
    auto itemList = onlyTopLevelItems(items());
    if (itemList.isEmpty()) return;

    if (data.isValid() && data.canConvert<QString>())
    {
      if (itemList.size() == 1 && isFrame(itemList.values().first()))
      {
        attemptUndoPush(new Commands::SetFrameTypeString((Frame*) itemList.values().first(), data.toString(), tr("change decoration")));
      }
      else
      {
        Frame *frame = new Frame();
        frame->setFrameString(data.toString());
        attemptBeginMacro(tr("add decoration"));
        Commands::ItemAction::addItemToScene(frame, scene());
        for(auto selectedItem : itemList)
          attemptUndoPush(new Commands::SetParentItem(selectedItem, frame));
        attemptEndMacro();
      }
    }
    else
    {
      QList<graphicsItem*> frameList;
      std::copy_if(itemList.begin(),
                   itemList.end(), std::back_inserter(frameList),
                   isFrame);
      if (frameList.isEmpty()) return;
      attemptBeginMacro(tr("remove decoration"));
      for (auto frame : frameList)
      {
        for (auto item : frame->childItems())
          attemptUndoPush(new Commands::SetParentItem(item, 0));
        Commands::ItemAction::removeItemFromScene(frame);
      }
      attemptEndMacro();
    }
  }

} // namespace Molsketch
