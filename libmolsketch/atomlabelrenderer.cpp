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

#include "atomlabelrenderer.h"

namespace Molsketch {

  class TextBox {
  protected:
    QPointF offset;
    QFont font;
    QFontMetrics metrics; // TODO should really be QFontMetricsF
    TextBox(const QPointF &offset, const QFont &font) : offset(offset), font(font), metrics(font) {}
  public:
    virtual void render(QPainter *painter) const = 0;
    virtual QRectF boundingRect() const = 0;
  };

  class RegularTextBox : public TextBox {
    QString text;
  public:
    RegularTextBox(const QString &text, const QPointF &offset, const QFont &font)
      : TextBox(offset, font), text(text) {}

    void render(QPainter *painter) const override {
      painter->save();
      painter->setFont(font);
      painter->drawText(offset, text);
      painter->restore();
    }

    QRectF boundingRect() const override {
      return QRectF(offset, QSizeF(metrics.width(text), metrics.height()))
          .translated(0, metrics.descent()-metrics.height()); // TODO replace with metrics.boundingRect()
//      return QRectF(metrics.boundingRect(text)).translated(offset); // TODO remove QRectF with QFontMetricsF
    }
  };

  class StackedTextBox : public TextBox {
    QString topText, bottomText;
    int shiftUp, shiftDown, mainHeight;
  public:
    StackedTextBox(const QString &topText,
                   const QString &bottomText,
                   const QPointF &offset,
                   const QFont & font,
                   const QFontMetrics & mainFontMetrics)
      : TextBox(offset, font),
        topText(topText),
        bottomText(bottomText),
        shiftUp(mainFontMetrics.ascent()),
        shiftDown(mainFontMetrics.descent()),
        mainHeight(mainFontMetrics.height())
    {}

    void render(QPainter *painter) const override {
      painter->save();
      painter->setFont(font);
      painter->drawText(offset + QPointF(0, shiftDown), bottomText);
      painter->drawText(offset + QPointF(0, shiftUp), topText);
      painter->restore();
    }

    QRectF boundingRect() const override {
      return QRectF(offset, QSizeF(qMax(metrics.width(topText), metrics.width(bottomText)), mainHeight))
          .translated(0, shiftDown - mainHeight);
      // TODO compute height correctly
    }
  };

  QVector<TextBox*> AtomLabelRenderer::generateTextBoxes(int alignment, const QString &lbl, const QPair<QFont, QFont>& fonts)
  {
    QFont symbolFont = fonts.first;
    QFont subscriptFont = fonts.second; // TODO get code from atom to fix second font
    QFontMetrics fmSymbol(symbolFont);
    QFontMetrics fmScript(subscriptFont);

    // compute the horizontal starting position
    qreal totalWidth = computeTotalWdith(alignment, lbl, fmSymbol, fmScript);
    qreal xOffset = computeXOffset(alignment, fmSymbol, lbl, totalWidth),
        yOffset = 0.5 * (fmSymbol.ascent() - fmSymbol.descent());
    QPointF currentPosition{xOffset, yOffset};
    qreal xInitial = xOffset;

    QRegularExpression boxContent{(alignment == Up || alignment == Down) ? "H?[A-GI-Za-z]+|[0-9]+|H" : "[A-Za-z]+|[0-9]+"};
    auto matches = boxContent.globalMatch(lbl);
    QVector<TextBox*> boxes;
    while (matches.hasNext()) {
      auto character = matches.next().captured();
      QRegularExpression number{"[0-9]+"};
      QRegularExpression hydrogens("^H[0-9]*$");

      if (character == 'H' && !hydrogens.match(lbl).hasMatch() && (alignment == Up || alignment == Down)) {
        currentPosition = QPointF(xInitial, currentPosition.y() +(alignment == Down ? fmSymbol.ascent() : - fmSymbol.ascent()));
        boxes << new RegularTextBox(character, currentPosition, symbolFont);
      } else if (number.match(character).hasMatch()) {
        boxes << new StackedTextBox("", character, currentPosition, subscriptFont, fmSymbol);
      } else {
        boxes << new RegularTextBox(character, currentPosition, symbolFont);
      }
      currentPosition.rx() = boxes.last()->boundingRect().right();
    }

    return boxes;
  }

  void AtomLabelRenderer::drawAtomLabel(QPainter *painter, const QString &lbl, const QPair<QFont, QFont>& fonts, int alignment)
  {
    auto boxes = generateTextBoxes(alignment, lbl, fonts);
    painter->save(); // TODO unite with computeBoundingRect
    std::for_each(boxes.begin(), boxes.end(), [&](const TextBox *box){ box->render(painter); });
    painter->restore();
  }

  QRectF AtomLabelRenderer::computeBoundingRect(const QString &lbl, const QPair<QFont, QFont> &fonts, Alignment alignment) {
    auto boxes = generateTextBoxes(alignment, lbl, fonts);
    QRectF result;
    std::for_each(boxes.begin(), boxes.end(), [&](const TextBox *box) { result |= box->boundingRect(); });
    return result;
  }

  qreal AtomLabelRenderer::computeTotalWdith(const int& alignment,
                                             const QString& lbl,
                                             const QFontMetrics &fmSymbol,
                                             const QFontMetrics &fmScript)
  {
    qreal totalWidth = 0;
    if ((alignment == Right) || (alignment == Left) || !lbl.contains("H")) {
      for (int i = 0; i < lbl.size(); ++i) {
        if (lbl[i].isDigit())
          totalWidth += fmScript.width(lbl[i]);
        else
          totalWidth += fmSymbol.width(lbl[i]);
      }
    } else {
      totalWidth = fmSymbol.width(lbl.left(lbl.indexOf("H")));
      qreal width = 0.0;
      for (int i = lbl.indexOf("H"); i < lbl.size(); ++i) {
        if (lbl[i].isDigit())
          width += fmScript.width(lbl[i]);
        else
          width += fmSymbol.width(lbl[i]);
      }

      if (width > totalWidth)
        totalWidth = width;
    }

    return totalWidth;
  }

  qreal AtomLabelRenderer::computeXOffset(int alignment, const QFontMetrics& fmSymbol, const QString& lbl, const qreal& totalWidth)
  {
    switch (alignment) {
      case Right:
        return - 0.5 * fmSymbol.width(lbl.left(1));
      case Left:
        return 0.5 * fmSymbol.width(lbl.right(1)) - totalWidth;
      case Up:
      case Down:
        if (lbl.contains("H") && !QRegExp("H[0-9]*").exactMatch(lbl))
          return - 0.5 * fmSymbol.width(lbl.left(lbl.indexOf("H")));
        [[fallthrough]];
      default:
        return - 0.5 * totalWidth;
    }
  }
} // namespace Molsketch
