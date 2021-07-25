#include "textline.h"
#include "textbox.h"
#include <algorithm>
#include <QPainter>

namespace Molsketch {

  QRectF TextLine::addRectFBefore(const QRectF &base, QRectF toAdd) const {
    if (!toAdd.isValid()) return base;
    toAdd.moveRight(base.left());
    return toAdd | base;
  }

  QRectF TextLine::addRectFAfter(const QRectF &base, QRectF toAdd) const {
    if (!toAdd.isValid()) return base;
    toAdd.moveLeft(base.right());
    return base | toAdd;
  }

  QPointF TextLine::beforeItemPreShift(const Paintable *item) const {
    auto bounds = item->boundingRect();
    return QPointF(-bounds.left() - bounds.width(), 0);
  }

  QPointF TextLine::beforeItemPostShift(const Paintable *item) const {
    Q_UNUSED(item)
    return QPointF();
  }

  QPointF TextLine::afterItemPreShift(const Paintable *item) const {
    return QPointF(-item->boundingRect().left(), 0);
  }

  QPointF TextLine::afterItemPostShift(const Paintable *item) const {
    return QPointF(item->boundingRect().right(), 0);
  }

  TextLine::TextLine(const TextBox *centerBox)
    : PaintableAggregate(centerBox) {}

  void TextLine::addBoxRight(const TextBox *newBox) {
    addAfter(newBox);
  }

  void TextLine::addBoxLeft(const TextBox *newBox) {
    addBefore(newBox);
  }

} // namespace Molsketch
