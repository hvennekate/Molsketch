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
#include <QSet>
#include <algorithm>
#include "alignmentaction.h"
#include "commands.h"
#include "iconutils.h"
#include <QDebug>

namespace Molsketch {

  AlignmentAction::AlignmentAction(const QString &description, MolScene *scene)
    : TopLevelItemAction(scene) {
    setCheckable(false);
    setMinimumItemCount(2);
    setText(description);
    // TODO when selecting a hidden atom with a molecule, it gets hidden again, but the molecule's boundingRect expands and remains so.
  }


  void AlignmentAction::execute() {
    QList<graphicsItem*> itemList = items();
    qreal targetValue = std::accumulate(itemList.begin(), itemList.end(),
                                        initialValue(), getAccumulator(itemList.size()));

    attemptBeginMacro(text());
    for (auto item : itemList) {
      // TODO this a workaround for molecules not using relative coordinates
      attemptUndoPush(
            new Commands::SetCoordinateCommand(item,item->coordinates().translated(getShift(item, targetValue))));
    }
    attemptEndMacro();
  }

#define ALIGNMENT_ACTION(DESCRIPTION, START, ICON, SHIFT, ACCUMULATOR) \
  class AlignmentActionImpl : public AlignmentAction { \
  Accumulator getAccumulator(int count) const override { Q_UNUSED(count) \
  return [=](const qreal& current, const graphicsItem* item) -> qreal { return ACCUMULATOR; } ; } \
  QPointF getShift(const graphicsItem* item, const qreal& targetValue) const override {return SHIFT; } \
  qreal initialValue() const override { return START; } \
  public: AlignmentActionImpl(MolScene *scene) : AlignmentAction(DESCRIPTION, scene) \
  { setIcon(getInternalIcon(ICON)); } \
}; \
  return new AlignmentActionImpl(scene);

  AlignmentAction *AlignmentAction::flushLeft(MolScene *scene) {
    ALIGNMENT_ACTION("Align left", qInf(), "align-left",
                     QPointF(targetValue - item->boundingRect().left(), 0),
                     qMin(current, item->boundingRect().left()))
  }

  AlignmentAction *AlignmentAction::flushRight(MolScene *scene) {
    ALIGNMENT_ACTION("Align right", -qInf(), "align-right",
                     QPointF(targetValue - item->boundingRect().right(), 0),
                     qMax(current, item->boundingRect().right()))
  }

  AlignmentAction *AlignmentAction::atTop(MolScene *scene) {
    ALIGNMENT_ACTION("Align top", qInf(), "align-top",
                     QPointF(0, targetValue - item->boundingRect().top()),
                     qMin(current, item->boundingRect().top()))
  }

  AlignmentAction *AlignmentAction::atBottom(MolScene *scene) {
    ALIGNMENT_ACTION("Align bottom", -qInf(), "align-bottom",
                     QPointF(0, targetValue - item->boundingRect().bottom()),
                     qMax(current, item->boundingRect().bottom()))
  }

  AlignmentAction *AlignmentAction::atVerticalCenter(MolScene *scene) {
    ALIGNMENT_ACTION("Align vertically", 0, "align-v-center",
                     QPointF(0, targetValue - item->boundingRect().center().y()),
                     current + item->boundingRect().center().y() / count)
  }

  AlignmentAction *AlignmentAction::atHorizontalCenter(MolScene *scene) {
    ALIGNMENT_ACTION("Align horizontally", 0, "align-h-center",
                     QPointF(targetValue - item->boundingRect().center().x(), 0),
                     current + item->boundingRect().center().x() / count)
  }
} // namespace Molsketch
