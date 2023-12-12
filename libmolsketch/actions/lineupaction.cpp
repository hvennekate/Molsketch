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
#include <QDebug>
#include "lineupaction.h"
#include "ui_alignrowdialog.h"
#include "molecule.h"
#include "commands.h"
#include "iconutils.h"

namespace Molsketch {

  LineUpAction *LineUpAction::horizontal(MolScene *scene) {
    class HorizontalLineUpAction : public LineUpAction {
      qreal getOrderingValue(const graphicsItem* item) const override {
        return item->boundingRect().left();
      }
      QPointF offsetForEdges(const graphicsItem* reference, const graphicsItem *item, qreal distance) const override {
        return QPointF(reference->boundingRect().right() + distance - item->boundingRect().left(), 0);
      }
      QPointF offsetForCenters(const graphicsItem* reference, const graphicsItem *item, qreal distance) const override {
        return QPointF(reference->boundingRect().center().x() + distance - item->boundingRect().center().x(), 0);
      }
    public:
      HorizontalLineUpAction(MolScene *scene) : LineUpAction(scene) {
        setIcon(getInternalIcon("space-horizontal"));
      }
    };

    return new HorizontalLineUpAction(scene);
  }

  LineUpAction *LineUpAction::vertical(MolScene *scene) {
    class VerticalLineUpAction : public LineUpAction {
      qreal getOrderingValue(const graphicsItem* item) const override {
        return item->boundingRect().top();
      }
      QPointF offsetForEdges(const graphicsItem* reference, const graphicsItem *item, qreal distance) const override {
        return QPointF(0, reference->boundingRect().bottom() + distance - item->boundingRect().top());
      }
      QPointF offsetForCenters(const graphicsItem* reference, const graphicsItem *item, qreal distance) const override {
        return QPointF(0, reference->boundingRect().center().y() + distance - item->boundingRect().center().y());
      }
    public:
      VerticalLineUpAction(MolScene *scene) : LineUpAction(scene) {
        setIcon(getInternalIcon("space-vertical"));
      }
    };

    return new VerticalLineUpAction(scene);
  }

  LineUpAction::LineUpAction(MolScene *scene)
    : TopLevelItemAction(scene) {
    setCheckable(false);
    setMinimumItemCount(2);
  }

  qreal calculateMinimumInterval(QList<graphicsItem*> items) {
    qreal minimumInterval = 0.;
    for (auto item : items) minimumInterval = qMax(minimumInterval, item->boundingRect().width());
    return minimumInterval;
  }

  void LineUpAction::execute() {
    qInfo() << "line up action called. Items:" << items();
    QDialog dialog;
    Ui::AlignRowDialog ui;
    ui.setupUi(&dialog);
    qreal minimumInterval = calculateMinimumInterval(items());
    ui.interval->setMinimum(minimumInterval);
    ui.interval->setMaximum(minimumInterval + 100);
    if (QDialog::Accepted != dialog.exec()) return;
    bool betweenCenters = ui.equalInterval->isChecked();
    spaceItemsEqually(betweenCenters ? ui.interval->value() : ui.distance->value(), betweenCenters);
  }

  void LineUpAction::spaceItemsEqually(qreal distance, bool distanceBetweenCenters) {
    qInfo() << "equally spacing items:" << distance << "between centers?" << distanceBetweenCenters;
    QList<graphicsItem*> items = LineUpAction::items();
    std::stable_sort(items.begin(), items.end(), [&](const graphicsItem* a, const graphicsItem* b)
    { return getOrderingValue(a) < getOrderingValue(b); });
    qDebug() << "Sorted items:" << items;
    if (items.size() < 2) return;
    attemptBeginMacro(tr("Space items equally"));
    auto previousItem = items.takeFirst();
    for (auto item : items) {
      attemptUndoPush(new Commands::SetCoordinateCommand(item, item->coordinates().translated(
                                                           distanceBetweenCenters
                                                           ? offsetForCenters(previousItem, item, distance)
                                                           : offsetForEdges(previousItem, item, distance))));
      previousItem = item;
    }
    attemptEndMacro();
  }
} // namespace Molsketch
