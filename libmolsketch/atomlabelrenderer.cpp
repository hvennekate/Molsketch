/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
 *   Copyright (C) 2019 Hendrik Vennekate                                  *
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

#include <QPainter>
#include <QRegularExpression>
#include <QDebug>
#include <numeric>
#include <algorithm>

#include <painting/textfield.h>
#include <painting/textline.h>

#include "atomlabelrenderer.h"
#include "painting/regulartextbox.h"
#include "painting/stackedtextbox.h"

namespace Molsketch {

  TextLine *hLine(int hAtomCount, const QFont &font) {
    auto line = new TextLine(new RegularTextBox("H", font));
    if (hAtomCount > 1) line->addBoxRight(new StackedTextBox("", QString::number(hAtomCount), font));
    return line;
  }

  TextField *AtomLabelRenderer::generateTextBoxes(const QString &lbl, const QFont &font, Alignment alignment, int hAtomCount, int charge)
  {
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

  void AtomLabelRenderer::drawAtomLabel(QPainter *painter, const QString &lbl, const QFont &font, int hAtomCount, Alignment alignment, int charge)
  {
    auto field = generateTextBoxes(lbl, font, alignment, hAtomCount, charge);
    painter->save(); // TODO unite with computeBoundingRect
    field->paint(painter);
    painter->restore();
    delete field;
  }

  QRectF AtomLabelRenderer::computeBoundingRect(const QString &lbl, const QFont &font, int hAtomCount, Alignment alignment, int charge) {
    auto field = generateTextBoxes(lbl, font, alignment, hAtomCount, charge);
    auto result = field->boundingRect();
    delete field;
    return result;
  }
} // namespace Molsketch
