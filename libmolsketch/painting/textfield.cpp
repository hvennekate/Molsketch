/***************************************************************************
 *   Copyright (C) 2022 by Hendrik Vennekate                               *
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

#include <QRegularExpression>
#include <QTransform>
#include "regulartextbox.h"
#include "stackedtextbox.h"
#include "textfield.h"
#include "textline.h"
#include <QDebug>

namespace Molsketch {

  const QRegularExpression NUMBER{"([0-9]+)"}; // TODO constants
  const QRegularExpression NUMBER_OR_NON_NUMBER("([0-9]+|[A-Z][a-z]*|[^0-9]+)");

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
    return QPointF(0, -bounds.center().y() + bounds.height());
  }

  QPointF TextField::afterItemPostShift(const Paintable *item) const {
    Q_UNUSED(item)
    return QPointF();
  }

  TextField::TextField(const TextLine *centerLine)
    : PaintableAggregate(centerLine) {}

  void TextField::addLineAbove(const TextLine *newLine) {
    addBefore(newLine);
  }

  void TextField::addLineBelow(const TextLine *newLine) {
    addAfter(newLine);
  }

  QString generateChargeString(int charge) {
    if (!charge) return "";

    return ((qAbs(charge) != 1) ? QString::number(qAbs(charge)) : QString())
        + ((charge > 0) ? QChar('+') : QChar(0x2212));
  }

  TextLine *hLine(int hAtomCount, const QFont &font, const QString &chargeString = QString()) {
    auto line = new TextLine(new RegularTextBox("H", font));
    if (hAtomCount > 1 || !chargeString.isEmpty())
      line->addBoxRight(new StackedTextBox(chargeString, QString::number(hAtomCount), font));
    return line;
  }

  TextField *TextField::generateLabelForAtom(const QString &lbl, const QFont &font, Alignment alignment, quint8 hAtomCount, int charge) {
    QString chargeLabel = generateChargeString(charge);
    bool placeChargeOnHydrogens = (hAtomCount && alignment == Alignment::Right);

    QVector<TextBox *> boxes;
    auto boxTexts = NUMBER_OR_NON_NUMBER.globalMatch(lbl);
    while (boxTexts.hasNext()) {
      auto boxText = boxTexts.next().captured(1);
      if (NUMBER.match(boxText).hasMatch()) {
        boxes << new StackedTextBox(!boxTexts.hasNext() && !placeChargeOnHydrogens ? chargeLabel : "",
                                    boxText, font);
      } else {
        boxes << new RegularTextBox(boxText, font);
        if (!boxTexts.hasNext() && !placeChargeOnHydrogens && !chargeLabel.isEmpty())
          boxes << new StackedTextBox(chargeLabel, "", font);
      }
    }

    if (boxes.empty()) return new TextField(nullptr); // TODO default constructor

    auto centerIterator = std::find_if(boxes.cbegin(), boxes.cend(), [](TextBox *box) { return box->preferredCenter(); });
    if (centerIterator == boxes.cend()) centerIterator = boxes.cbegin();

    auto line = new TextLine(*centerIterator);
    if (centerIterator != boxes.cbegin()) {
      QVector<TextBox *> leftBoxes(centerIterator - boxes.cbegin());
      std::copy_backward(boxes.cbegin(), centerIterator, leftBoxes.end());
      for (auto leftBox : leftBoxes) line->addBoxLeft(leftBox);
    }
    for (auto rightBoxIterator = centerIterator + 1; rightBoxIterator != boxes.cend(); ++rightBoxIterator) {
        line->addBoxRight(*rightBoxIterator);
    }

    auto field = new TextField(line);

    if (hAtomCount) {
      switch(alignment) {
        case Alignment::Up: field->addLineAbove(hLine(hAtomCount, font)); break;
        case Alignment::Down: field->addLineBelow(hLine(hAtomCount, font)); break;
        case Alignment::Left: {
            if (hAtomCount > 1) line->addBoxLeft(new StackedTextBox("", QString::number(hAtomCount), font));
            line->addBoxLeft(new RegularTextBox("H", font));
            break;
          }
        case Alignment::Right: {
            line->addBoxRight(new RegularTextBox("H", font));
            if (hAtomCount > 1 || !chargeLabel.isEmpty())
              line->addBoxRight(new StackedTextBox(chargeLabel,
                                                   hAtomCount > 1 ? QString::number(hAtomCount) : "", font));
            break;
          }
      }
    }

    return field;
  }
} // namespace Molsketch
