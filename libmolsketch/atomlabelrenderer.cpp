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
    qreal totalWidth = 0.0;
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

    QString str, subscript;
    // compute the horizontal starting position
    qreal xOffset, yOffset, yOffsetSubscript;
    switch (alignment) {
    case Right:
      xOffset = - 0.5 * fmSymbol.width(lbl.left(1));
      break;
    case Left:
      xOffset = 0.5 * fmSymbol.width(lbl.right(1)) - totalWidth;
      break;
    case Up:
    case Down:
      if (lbl.contains("H") && !QRegExp("H[0-9]*").exactMatch(lbl))
        xOffset = - 0.5 * fmSymbol.width(lbl.left(lbl.indexOf("H")));
      else
        xOffset = - 0.5 * totalWidth;
      break;
    default:
      xOffset = - 0.5 * totalWidth;
      break;
    }
    // compute the vertical starting position
    yOffset = 0.5 * (fmSymbol.ascent() - fmSymbol.descent());
    yOffsetSubscript = yOffset + fmSymbol.descent();
    qreal xInitial = xOffset;

    for (int i = 0; i < lbl.size(); ++i) {
      if (lbl[i] == 'H') {
        if ((alignment == Up) || (alignment == Down))
          if (!str.isEmpty()) {
            // write the current string
            painter->setFont(symbolFont);
            painter->drawText(xOffset, yOffset, str);
            if (alignment == Down) {
              yOffset += fmSymbol.ascent()/* - fmSymbol.descent()*/;
              yOffsetSubscript += fmSymbol.ascent()/* - fmSymbol.descent()*/;
            } else {
              yOffset -= fmSymbol.ascent()/* + fmSymbol.descent()*/;
              yOffsetSubscript -= fmSymbol.ascent()/* + fmSymbol.descent()*/;
            }
            xOffset = xInitial;
            str.clear();
          }
      }

      if (lbl[i].isDigit()) {
        if (!str.isEmpty()) {
          // write the current string
          painter->setFont(symbolFont);
          painter->drawText(xOffset, yOffset, str);
          xOffset += fmSymbol.width(str);
          str.clear();
        }

        subscript += lbl.mid(i, 1);
      } else {
        if (!subscript.isEmpty()) {
          // write the current subscript
          painter->setFont(subscriptFont);
          painter->drawText(xOffset, yOffsetSubscript, subscript);
          xOffset += fmScript.width(subscript);
          subscript.clear();
        }

        str += lbl.mid(i, 1);
      }
    }
    if (!str.isEmpty()) {
      painter->setFont(symbolFont);
      painter->drawText(xOffset, yOffset, str);
    }
    if (!subscript.isEmpty()) {
      painter->setFont(subscriptFont);
      painter->drawText(xOffset, yOffsetSubscript, subscript);
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
    qreal xOffset;
    switch (alignment) {
      case Right:
        xOffset = - 0.5 * fmSymbol.width(lbl.left(1));
        break;
      case Left:
        xOffset = 0.5 * fmSymbol.width(lbl.right(1)) - totalWidth;
        break;
      case Up:
      case Down:
        if (lbl.contains("H") && !QRegExp("H[0-9]*").exactMatch(lbl))
          xOffset = - 0.5 * fmSymbol.width(lbl.left(lbl.indexOf("H")));
        else
          xOffset = - 0.5 * totalWidth;
        break;
      default:
        xOffset = - 0.5 * totalWidth;
        break;
    }
    return xOffset;
  }
} // namespace Molsketch
