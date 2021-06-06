#include "textline.h"
#include "textbox.h"
#include <algorithm>
#include <QDebug>

namespace Molsketch {

  class TextLine::TextLinePrivate {
  public:
    QList<const TextBox*> leftBoxes, rightBoxes;
    QScopedPointer<const TextBox> centerBox;
    explicit TextLinePrivate(const TextBox *centerBox) : centerBox(centerBox) {}
    ~TextLinePrivate() {
      for (auto box : leftBoxes + rightBoxes) delete box;
    }
  };

  TextLine::TextLine(const TextBox *centerBox)
    : d(new TextLinePrivate(centerBox))
  {}

  TextLine::~TextLine() {
    delete d;
  }

  QRectF addToLeft(const QRectF &base, QRectF toAdd) {
    if (!toAdd.isValid()) return base;
    toAdd.moveRight(base.left());
    return toAdd | base;
  }

  QRectF addToRight(const QRectF &base, QRectF toAdd) {
    if (!toAdd.isValid()) return base;
    toAdd.moveLeft(base.right());
    return base | toAdd;
  }

  QRectF boundingRectFromPointer(const TextBox *p) {
    return p->boundingRect();
  }

  QRectF TextLine::boundingBox() const {
    auto resultBox = std::transform_reduce(d->leftBoxes.cbegin(), d->leftBoxes.cend(), d->centerBox->boundingRect(), addToLeft,
                                           boundingRectFromPointer);
    resultBox = std::transform_reduce(d->rightBoxes.cbegin(), d->rightBoxes.cend(), resultBox, addToRight,
                                      boundingRectFromPointer);
    // TODO determine baseline
    return resultBox.translated(- d->centerBox->boundingRect().center());
  }

  void TextLine::render(QPainter *painter) const {
    painter->save();
    QPointF offset(-d->centerBox->boundingRect().center());
    auto centerTransform = QTransform(painter->transform()).translate(offset.x(), offset.y());
    painter->setTransform(centerTransform);
    d->centerBox->render(painter);

    painter->setTransform(QTransform(centerTransform).translate(d->centerBox->boundingRect().width(), 0));
    for (auto box : qAsConst(d->rightBoxes)) {
      auto bounds = box->boundingRect();
      painter->setTransform(QTransform(painter->transform()).translate(-bounds.left(), 0));
      box->render(painter);
      painter->setTransform(QTransform(painter->transform()).translate(bounds.right(), 0));
    }

    painter->setTransform(centerTransform);
    for (auto box : qAsConst(d->leftBoxes)) {
      auto bounds = box->boundingRect();
      painter->setTransform(QTransform(painter->transform()).translate(-bounds.left() - bounds.width(), 0));
      box->render(painter);
    }

    painter->restore();
  }

  void TextLine::addBoxRight(const TextBox *newBox) {
    d->rightBoxes << newBox;
  }

  void TextLine::addBoxLeft(const TextBox *newBox) {
    d->leftBoxes << newBox;
  }

} // namespace Molsketch
