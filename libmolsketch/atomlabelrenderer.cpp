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
#include <numeric>

#include "atomlabelrenderer.h"

namespace Molsketch {

  void AtomLabelRenderer::drawAtomLabel(QPainter *painter, const QString &lbl, const QPair<QFont, QFont>& fonts, int alignment)
  {
    painter->save(); // TODO unite with computeBoundingRect
    QFont symbolFont = fonts.first;
    QFont subscriptFont = fonts.second;
    QFontMetrics fmSymbol(symbolFont);
    QFontMetrics fmScript(subscriptFont);

    // compute the total width
    qreal totalWidth = computeTotalWdith(alignment, lbl, fmSymbol, fmScript);

    // compute the horizontal starting position
    qreal xOffset = computeXOffset(alignment, fmSymbol, lbl, totalWidth),
        yOffset = 0.5 * (fmSymbol.ascent() - fmSymbol.descent()),
        yOffsetSubscript = yOffset + fmSymbol.descent();

    // compute the vertical starting position
    qreal xInitial = xOffset;

    QRegularExpression boxContent{(alignment == Up || alignment == Down) ? "H?[A-GI-Za-z]+|[0-9]+|H" : "[A-Za-z]+|[0-9]+"};
    auto matches = boxContent.globalMatch(lbl);
    while (matches.hasNext()) {
      auto character = matches.next().captured();
      QRegularExpression number{"[0-9]+"};
      QRegularExpression hydrogens("^H[0-9]*$");

      if (character == 'H' && !hydrogens.match(lbl).hasMatch() && (alignment == Up || alignment == Down)) {
          painter->setFont(symbolFont);
          if (alignment == Down) {
            yOffset += fmSymbol.ascent();
            yOffsetSubscript += fmSymbol.ascent();
          } else {
            yOffset -= fmSymbol.ascent();
            yOffsetSubscript -= fmSymbol.ascent();
          }
          xOffset = xInitial;
        painter->drawText(xOffset, yOffset, character);
      } else if (number.match(character).hasMatch()) {
        painter->setFont(subscriptFont);
        painter->drawText(xOffset, yOffsetSubscript, character);
        xOffset += fmScript.width(character);
      } else {
        painter->setFont(symbolFont);
        painter->drawText(xOffset, yOffset, character);
        xOffset += fmSymbol.width(character);
      }
    }
    painter->restore();
  }

  QRectF AtomLabelRenderer::computeBoundingRect(const QString &lbl, const QPair<QFont, QFont> &fonts, Alignment alignment) {
    if (fonts.first.pointSizeF() < 0) return QRectF();
    QFontMetrics fmSymbol(fonts.first), fmScript(fonts.second);

    qreal totalWidth = computeTotalWdith(alignment, lbl, fmSymbol, fmScript);
    qreal xOffset = computeXOffset(alignment, fmSymbol, lbl, totalWidth);
    qreal yOffset = 0.5 * (fmSymbol.ascent() - fmSymbol.descent());
    qreal yOffsetSubscript = yOffset + fmSymbol.descent();

    // compute the shape
    if ((alignment == Right) || (alignment == Left) || !lbl.contains("H") || QRegExp("H[0-9]*").exactMatch(lbl))
      return QRectF(xOffset, yOffsetSubscript - fmSymbol.height(), totalWidth, fmSymbol.height());
    if (alignment == Down)
      return QRectF(xOffset, yOffsetSubscript - fmSymbol.height(), totalWidth, fmSymbol.ascent() + fmSymbol.height());
    return QRectF(xOffset, yOffsetSubscript - fmSymbol.ascent() - fmSymbol.height(), totalWidth, fmSymbol.ascent() + fmSymbol.height());
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
