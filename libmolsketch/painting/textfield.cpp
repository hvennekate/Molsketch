#include <QTransform>
#include "textfield.h"
#include "textline.h"

namespace Molsketch {

  // TODO align horizontally
  QRectF TextField::addRectFBefore(const QRectF &base, QRectF toAdd) const {
    if (!toAdd.isValid()) return base;
    toAdd.moveBottom(base.top());
    return toAdd | base;
  }

  QRectF TextField::addRectFAfter(const QRectF &base, QRectF toAdd) const {
    if (!toAdd.isValid()) return base;
    toAdd.moveTop(base.bottom());
    return toAdd | base;
  }

  QPointF TextField::beforeItemPreShift(const Paintable *item) const {
    auto bounds = item->boundingRect();
    return QPointF(0, -bounds.center().y() - bounds.height());
  }

  QPointF TextField::beforeItemPostShift(const Paintable *item) const {
    Q_UNUSED(item)
    return QPointF();
  }

  QPointF TextField::afterItemPreShift(const Paintable *item) const {
    auto bounds = item->boundingRect();
    return QPointF(0, -bounds.center().y() + bounds.height()); //-item->boundingRect().top());
  }

  QPointF TextField::afterItemPostShift(const Paintable *item) const {
    return QPointF(0, 0); //item->boundingRect().bottom());
  }

  TextField::TextField(const TextLine *centerLine)
    : PaintableAggregate(centerLine) {}

  void TextField::addLineAbove(const TextLine *newLine) {
    addBefore(newLine);
  }

  void TextField::addLineBelow(const TextLine *newLine) {
    addAfter(newLine);
  }
} // namespace Molsketch
