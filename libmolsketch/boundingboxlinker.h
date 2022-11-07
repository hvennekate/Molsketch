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
#ifndef MOLSKETCH_BOUNDINGBOXLINKER_H
#define MOLSKETCH_BOUNDINGBOXLINKER_H

#include "abstractxmlobject.h"

#include <QPointF>
#include <QRectF>
#include <QScopedPointer>
#include <QtGlobal>

namespace Molsketch { // TODO turn on warnings for switch'es
  enum class Anchor : int {
    TopLeft = 0,
    Top = 1,
    TopRight = 2,
    Left = 4,
    BottomLeft = 8,
    Bottom = 9,
    Center = 5,
    Right = 6,
    BottomRight = 10
  };

  int convertAnchor(const Anchor& anchor);
  Anchor convertAnchor(const int& integer);
  Anchor anchorFromString(QString input);
  QString toString(const Anchor& anchor);
#ifdef QT_DEBUG
  QDebug operator<<(QDebug debug, const Anchor& anchor);
#endif

  class Linkable {
  public:
    virtual QPointF getAnchorPoint(const Anchor &anchor) const = 0;
    static QPointF getAnchorPoint(const QRectF &, const Anchor &anchor);
  };

  class BoundingBoxLinkerPrivate;

  class BoundingBoxLinker : public abstractXmlObject {
    Q_DECLARE_PRIVATE(BoundingBoxLinker)
    QScopedPointer<BoundingBoxLinkerPrivate> d_ptr;
  public:
    explicit BoundingBoxLinker(Anchor originAnchor = Anchor::Center, Anchor ownAnchor = Anchor::Center, const QPointF& offset = QPointF());
    BoundingBoxLinker(const BoundingBoxLinker& other);
    virtual ~BoundingBoxLinker();
    QPointF getShift(const QRectF &reference, const QRectF &target) const;
    QPointF getShift(const Linkable &reference, const Linkable &target) const;
    QString xmlName() const override;
    static QString xmlClassName();
    QXmlStreamAttributes xmlAttributes() const override;
    void readAttributes(const QXmlStreamAttributes& attributes) override;
    bool operator==(const BoundingBoxLinker& other) const;
#ifdef QT_DEBUG
    friend QDebug operator<<(QDebug debug, const BoundingBoxLinker& linker);
#endif
    BoundingBoxLinker& operator=(const BoundingBoxLinker& other);
    static BoundingBoxLinker upperLeft();
    static BoundingBoxLinker lowerLeft();
    static BoundingBoxLinker upperRight();
    static BoundingBoxLinker lowerRight();
    static BoundingBoxLinker above();
    static BoundingBoxLinker below();
    static BoundingBoxLinker toRight();
    static BoundingBoxLinker toLeft();

    static BoundingBoxLinker atTopLeft();
    static BoundingBoxLinker atBottomLeft();
    static BoundingBoxLinker atTopRight();
    static BoundingBoxLinker atBottomRight();
    static BoundingBoxLinker atTop();
    static BoundingBoxLinker atBottom();
    static BoundingBoxLinker atLeft();
    static BoundingBoxLinker atRight();
  };

#ifdef QT_DEBUG
  QDebug operator<<(QDebug debug, const BoundingBoxLinker& linker);
#endif

} // namespace Molsketch

#endif // MOLSKETCH_BOUNDINGBOXLINKER_H
