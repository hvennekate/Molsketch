#include <QRegularExpression>
#include <QTransform>
#include "regulartextbox.h"
#include "stackedtextbox.h"
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

  TextLine *hLine(int hAtomCount, const QFont &font) {
    auto line = new TextLine(new RegularTextBox("H", font));
    if (hAtomCount > 1) line->addBoxRight(new StackedTextBox("", QString::number(hAtomCount), font));
    return line;
  }

  TextField *TextField::generateLabelForAtom(const QString &lbl, const QFont &font, Alignment alignment, quint8 hAtomCount, int charge) {
    QRegularExpression number{"([0-9]+)"};
    QRegularExpression numberOrNonNumber("([0-9]+|[^0-9]+)");

    QVector<TextBox *> boxes;
    auto boxTexts = numberOrNonNumber.globalMatch(lbl);
    while (boxTexts.hasNext()) {
      auto boxText = boxTexts.next().captured(1);
      if (number.match(boxText).hasMatch())
        boxes << new StackedTextBox("", boxText, font);
      else
        boxes << new RegularTextBox(boxText, font);
    }

    if (boxes.empty()) return new TextField(nullptr); // TODO default constructor

    auto centerIterator = std::find_if(boxes.cbegin(), boxes.cend(), [](TextBox *box) { return box->preferredCenter(); });
    if (centerIterator == boxes.cend()) centerIterator = boxes.cbegin();

    auto line = new TextLine(*centerIterator);
    if (centerIterator != boxes.cbegin())
      for (auto leftBoxIterator = centerIterator - 1; leftBoxIterator != boxes.cbegin(); --leftBoxIterator)
        line->addBoxLeft(*leftBoxIterator);
    for (auto rightBoxIterator = centerIterator + 1; rightBoxIterator != boxes.cend(); ++rightBoxIterator)
      line->addBoxRight(*rightBoxIterator);

    auto field = new TextField(line);

    auto topLine = line;
    if (hAtomCount >= 1) {
      switch(alignment) {
        case Alignment::Up: field->addLineAbove(topLine = hLine(hAtomCount, font)); break;
        case Alignment::Down: field->addLineBelow(hLine(hAtomCount, font)); break;
        case Alignment::Left: {
            if (hAtomCount > 1) line->addBoxLeft(new StackedTextBox("", QString::number(hAtomCount), font));
            line->addBoxLeft(new RegularTextBox("H", font)); break;
          }
        case Alignment::Right: {
            line->addBoxRight(new RegularTextBox("H", font));
            if (hAtomCount > 1) line->addBoxRight(new StackedTextBox("", QString::number(hAtomCount), font)); break;
          }
      }
    }

    if (!charge) return field;

    QString chargeLabel;
    if (qAbs(charge) != 1) chargeLabel += QString::number(charge);
    chargeLabel += (charge > 0) ? "+" : "-";
    topLine->addBoxRight(new StackedTextBox(chargeLabel, "", font)); // TODO should be part of last label/stacked text box

    return field;
  }
} // namespace Molsketch
