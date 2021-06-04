#include "textline.h"
#include "textbox.h"
#include <algorithm>

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
    // TODO
  }

  void TextLine::addBoxRight(const TextBox *newBox) {
    d->rightBoxes << newBox;
  }

  void TextLine::addBoxLeft(const TextBox *newBox) {
    d->leftBoxes << newBox;
  }

} // namespace Molsketch
