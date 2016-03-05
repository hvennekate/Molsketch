/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#define ATOM_SIZE 5

#include "atom.h"

#include <QPainter>
#include <QSettings>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "bond.h"
#include "molecule.h"

#include "element.h"
#include "molscene.h"
#include "TextInputItem.h"
#include <iostream>
#include <cmath>

namespace Molsketch {

  //                                        //
  //     /    \      \   /      H           //
  //   HN      NH      N        N           //
  //     \    /        H      /   \         //
  //                                        //
  //   Left   Right   Down     Up           //
  //                                        //
  enum {
    Left,
    Right,
    Up,
    Down
  };

  int Atom::labelAlignment() const
  {
    // compute the sum of the bond vectors, this gives
    QPointF direction(0.0, 0.0);
    foreach (Atom *nbr, this->neighbours())
      direction += this->pos() - nbr->pos();

    int alignment = 0;
    if ((this->numBonds() == 2) && (abs(direction.y()) > abs(direction.x()))) {
      if (direction.y() <= 0.0)
        alignment = Up;
      else
        alignment = Down;
    } else {
      //qDebug() << "x =" << direction.x();
      if (direction.x() < -0.1) // hack to make almost vertical lines align Right
        alignment = Left;
      else
        alignment = Right;
    }

    return alignment;
  }


  Atom::Atom(const QPointF &position, const QString &element, bool implicitHydrogens,
     QGraphicsItem* parent GRAPHICSSCENESOURCE )
    : graphicsItem (parent GRAPHICSSCENEINIT )
  {
    initialize(position, element, implicitHydrogens);
  }

  Atom::Atom(const Atom &other GRAPHICSSCENESOURCE)
    : graphicsItem (other GRAPHICSSCENEINIT)
  {
    initialize(other.scenePos(), other.element(), other.hasImplicitHydrogens());
  }

  Atom::~Atom()
  {
  }


  //////////////////////////////////////////////////////////////////////////////
  // Inherited painting methods
  //////////////////////////////////////////////////////////////////////////////

  qreal Atom::computeTotalWdith(const int& alignment,
                                const QString& lbl,
                                const QFontMetrics &fmSymbol,
                                const QFontMetrics &fmScript)
  {
    qreal totalWidth = 0;
    // compute the total width
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

  qreal Atom::computeXOffset(int alignment, const QFontMetrics& fmSymbol, const QString& lbl, const qreal& totalWidth)
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
        if (lbl.contains("H"))
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

  QPair<QFont, QFont> Atom::getFonts() const
  {
    QFont symbolFont = getSymbolFont();
    return qMakePair(symbolFont, getSubscriptFont(symbolFont));
  }

  QFont Atom::getSymbolFont() const
  {
    QSettings settings;
    QFont symbolFont = settings.value("atom-symbol-font").value<QFont>();
    symbolFont.setPointSizeF(symbolFont.pointSizeF() * relativeWidth());
    return symbolFont;
  }

  QFont Atom::getSubscriptFont(const QFont& symbolFont) const
  {
    QFont subscriptFont = symbolFont;
    subscriptFont.setPointSize(0.75 * symbolFont.pointSize());
    return subscriptFont;
  }

  QString Atom::composeLabel(bool leftAligned) const
  {
    int hCount = numImplicitHydrogens();
    QString lbl;
    if (hCount && leftAligned)
      lbl += "H";
    if ((hCount > 1) && leftAligned)
      lbl += QString::number(hCount);

    lbl += m_elementSymbol;

    if (hCount && !leftAligned)
      lbl += "H";
    if ((hCount > 1) && !leftAligned)
      lbl += QString::number(hCount);
    return lbl;
  }

  QRectF Atom::computeBoundingRect()
  {
    // TODO do proper prepareGeometryChange() call
    // TODO call whenever boundingRect() is called
    int alignment = labelAlignment();

    QString lbl = composeLabel(Left == alignment);

    QPair<QFont, QFont> fonts = getFonts();
    QFontMetrics fmSymbol(fonts.first), fmScript(fonts.second);

    qreal totalWidth = computeTotalWdith(alignment, lbl, fmSymbol, fmScript);
    qreal xOffset = computeXOffset(alignment, fmSymbol, lbl, totalWidth);
    qreal yOffset = 0.5 * (fmSymbol.ascent() - fmSymbol.descent());
    qreal yOffsetSubscript = yOffset + fmSymbol.descent();

    // compute the shape
    if ((alignment == Right) || (alignment == Left) || !lbl.contains("H"))
      return QRectF(xOffset, yOffsetSubscript - fmSymbol.height(), totalWidth, fmSymbol.height());
    if (alignment == Down)
      return QRectF(xOffset, yOffsetSubscript - fmSymbol.height(), totalWidth, fmSymbol.ascent() + fmSymbol.height());
    return QRectF(xOffset, yOffsetSubscript - fmSymbol.ascent() - fmSymbol.height(), totalWidth, fmSymbol.ascent() + fmSymbol.height());
  }

  void Atom::initialize(const QPointF &position,
                        const QString &element,
                        bool implicitHydrogens)
  {
    //pre: position is a valid position in scene coordinates
    setPos(position);
    setZValue(3);

    MolScene *molScene = dynamic_cast<MolScene*>(scene()); // @todo qobject_cast is faster

    if (molScene) {
      setColor (molScene ->color());    // Setting initial parameters
    }
    else setColor (QColor (0, 0, 0));
    // Enabling hovereffects
#if QT_VERSION < 0x050000
    setAcceptsHoverEvents(true);
#else
    setAcceptHoverEvents(true) ;
#endif

    // Setting private fields
    m_elementSymbol = element;
    m_hidden = true;

    m_userCharge = 0; // The initial additional charge is zero
    m_userElectrons = 0;
    m_userImplicitHydrogens =  0;
    enableImplicitHydrogens(implicitHydrogens);
    m_shape = computeBoundingRect();
  }

  QRectF Atom::boundingRect() const
  {
    if (!isDrawn()) return QRect();
    return m_shape;
  }

  bool Atom::hasLabel() const
  {
    MolScene* molScene = dynamic_cast<MolScene*>(scene());
    if (!molScene) return true ;

    if ((m_elementSymbol == "C") && !molScene->carbonVisible() && (numBonds() > 1) && ((charge() == 0) || !molScene->chargeVisible()))
      return false;

    return true;
  }

  void Atom::drawAtomLabel(QPainter *painter, const QString &lbl, int alignment)
  {
    MolScene* molScene = dynamic_cast<MolScene*>(scene());

    painter->save(); // TODO mit computeBoundingRect zusammenfuehren
    QFont symbolFont = molScene->atomFont();
    symbolFont.setPointSizeF(symbolFont.pointSizeF()*relativeWidth());
    QFont subscriptFont = symbolFont;
    subscriptFont.setPointSize(0.75 * symbolFont.pointSize());
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

    // debug
    /*
    painter->save();
    painter->setPen(Qt::red);
    painter->drawPoint(QPointF(0.0, 0.0));
    painter->restore();
    */

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
        if (lbl.contains("H"))
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

    // compute the shape
    if ((alignment == Right) || (alignment == Left) || !lbl.contains("H"))
      m_shape = QRectF(xOffset, yOffsetSubscript - fmSymbol.height(), totalWidth, fmSymbol.height());
    else {
      if (alignment == Down)
        m_shape = QRectF(xOffset, yOffsetSubscript - fmSymbol.height(), totalWidth, fmSymbol.ascent() + fmSymbol.height());
      else
        m_shape = QRectF(xOffset, yOffsetSubscript - fmSymbol.ascent() - fmSymbol.height(), totalWidth, fmSymbol.ascent() + fmSymbol.height());
    }

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


  void Atom::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
  {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->setPen(getColor());
    // Save the original painter state
    //painter->save();
    
    /*    
          qDebug() << "Atom::paint()";
          qDebug() << "m_hidden" << m_hidden;
          qDebug() << "isSelected" << isSelected();
          qDebug() << "numBonds" << numBonds();
    */
    // Check the scene
    MolScene* molScene = dynamic_cast<MolScene*>(scene());
    if (!molScene) return ;
    Q_CHECK_PTR(molScene);

    int element = Element::strings.indexOf(m_elementSymbol);
    
    switch (molScene->renderMode()) {
      case MolScene::RenderColoredSquares:
        if (element != Element::C) {
          QColor color = elementColor(element);
          painter->setPen(color);
          painter->setBrush(color);
          qreal half = 10.0;
          painter->drawRect(-half, -half, 2.0 * half, 2.0 * half);
        }
        return;
      case MolScene::RenderColoredCircles:
        if (element != Element::C) {
          QColor color = elementColor(element);
          painter->setPen(color);
          painter->setBrush(color);
          qreal half = 10.0;
          painter->drawEllipse(-half, -half, 2.0 * half, 2.0 * half);
        }
        return;
      case MolScene::RenderColoredWireframe:
        return;
      default:
      case MolScene::RenderLabels:
        break;
    }

    if (!isDrawn()) return;

    int alignment = labelAlignment();
    bool leftAligned = false;
    switch (alignment) {
      case Left:
        leftAligned = true;
      default:
        break;
    }

    int hCount = numImplicitHydrogens();

    QString lbl;
    if (hCount && leftAligned)
      lbl += "H";
    if ((hCount > 1) && leftAligned)
      lbl += QString::number(hCount);

    lbl += m_elementSymbol;
    
    if (hCount && !leftAligned)
      lbl += "H";
    if ((hCount > 1) && !leftAligned)
      lbl += QString::number(hCount);

    painter->setPen(getColor());

    drawAtomLabel(painter, lbl, alignment);

    // Drawing background
    if (this->isSelected()) {
      painter->save();
      painter->setPen(Qt::blue);
      painter->drawRect(m_shape);
      painter->restore();
    }

    // Draw charge
    if (molScene->chargeVisible()) {
        QString chargeId = chargeString();
        QFont superscriptFont = molScene->atomFont();
        superscriptFont.setPointSize(0.5 * superscriptFont.pointSize());
        QFontMetrics fmSymbol(superscriptFont);
        int offset = 0.5 * fmSymbol.width("+");
        painter->drawText(m_shape.right() - offset, m_shape.top() + offset, chargeId);
    }

    if (molScene->lonePairsVisible()) {
      // Draw unbound electrons
      int unboundElectrons = numNonBondingElectrons();
      QList<QRectF> layoutList;

      if (m_bonds.size() == 0) {
        //  ..   ..   ..     ..
        //  NH2  OH2  CH3    N .
        //       ''          ''
        layoutList << QRectF(-3,-10,2,2); // top1
        layoutList << QRectF(3,-10,2,2); // top2
        layoutList << QRectF(-3,10,2,2); // bottom1
        layoutList << QRectF(3,10,2,2); // bottom2
        layoutList << QRectF(-10,-3,2,2); // left1
        layoutList << QRectF(-10,3,2,2); // left2
        layoutList << QRectF(10,-3,2,2); // right1
        layoutList << QRectF(10,3,2,2); // right2
      } else if (m_bonds.size() == 1) {
        //   ..   ..     ..    |
        // --OH   HO--  :OH   :OH
        //   ''   ''     |     ''
        QPointF direction(0.0, 0.0);
        foreach (Atom *nbr, neighbours())
          direction += pos() - nbr->pos();

        if (abs(direction.y()) > abs(direction.x())) {
          if (direction.y() <= 0.0) {
            //   ..
            //   NH2
            //   |
            layoutList << QRectF(-3,-10,2,2); // top1
            layoutList << QRectF(3,-10,2,2); // top2
            if (direction.x() < -0.1) {
              layoutList << QRectF(10,-3,2,2); // right1
              layoutList << QRectF(10,3,2,2); // right2
              layoutList << QRectF(-10,-3,2,2); // left1
              layoutList << QRectF(-10,3,2,2); // left2
            } else {
              layoutList << QRectF(-10,-3,2,2); // left1
              layoutList << QRectF(-10,3,2,2); // left2
              layoutList << QRectF(10,-3,2,2); // right1
              layoutList << QRectF(10,3,2,2); // right2
            }
            layoutList << QRectF(-3,10,2,2); // bottom1
            layoutList << QRectF(3,10,2,2); // bottom2
          } else {
            layoutList << QRectF(-3,10,2,2); // bottom1
            layoutList << QRectF(3,10,2,2); // bottom2
            if (direction.x() < -0.1) {
              layoutList << QRectF(10,-3,2,2); // right1
              layoutList << QRectF(10,3,2,2); // right2
              layoutList << QRectF(-10,-3,2,2); // left1
              layoutList << QRectF(-10,3,2,2); // left2
            } else {
              layoutList << QRectF(-10,-3,2,2); // left1
              layoutList << QRectF(-10,3,2,2); // left2
              layoutList << QRectF(10,-3,2,2); // right1
              layoutList << QRectF(10,3,2,2); // right2
            }
            layoutList << QRectF(-3,-10,2,2); // top1
            layoutList << QRectF(3,-10,2,2); // top2
          }
        } else {
          if (direction.x() < 0.0) {
            layoutList << QRectF(-3,-10,2,2); // top1
            layoutList << QRectF(3,-10,2,2); // top2
            layoutList << QRectF(-3,10,2,2); // bottom1
            layoutList << QRectF(3,10,2,2); // bottom2
            layoutList << QRectF(-10,-3,2,2); // left1
            layoutList << QRectF(-10,3,2,2); // left2
            layoutList << QRectF(10,-3,2,2); // right1
            layoutList << QRectF(10,3,2,2); // right2

          } else {
            layoutList << QRectF(-3,-10,2,2); // top1
            layoutList << QRectF(3,-10,2,2); // top2
            layoutList << QRectF(-3,10,2,2); // bottom1
            layoutList << QRectF(3,10,2,2); // bottom2
            layoutList << QRectF(10,-3,2,2); // right1
            layoutList << QRectF(10,3,2,2); // right2
            layoutList << QRectF(-10,-3,2,2); // left1
            layoutList << QRectF(-10,3,2,2); // left2
          }
        }

      }

      if (layoutList.isEmpty()) {
        // Loading different layouts
        layoutList << QRectF(-3,-10,2,2); // bottom1
        layoutList << QRectF(3,-10,2,2); // bottom2
        layoutList << QRectF(-3,10,2,2); // top1
        layoutList << QRectF(3,10,2,2); // top2
        layoutList << QRectF(10,-3,2,2); // right1
        layoutList << QRectF(10,3,2,2); // right2
        layoutList << QRectF(-10,-3,2,2); // left1
        layoutList << QRectF(-10,3,2,2); // left2
      }


      painter->save();
//      painter->setBrush(Qt::black);

      for (int i = 0; i < unboundElectrons; i++)
        painter->drawEllipse(layoutList[i]);

      painter->restore();
    }
  }

  qreal Atom::annotationDirection() const
  {
    // Determine optimum direction if angleDirection negative
    //   No preference & no bonds => downward
    if (m_bonds.isEmpty())
      return 270 ;
    if (m_bonds.size() == 1)
      return Molecule::toDegrees(m_bonds.first()->bondAngle(this)+180.) ;
    //   Have bonds? determine largest free angle
    QVector<qreal> angles ;
    foreach (Bond *bond, m_bonds)
      angles << bond->bondAngle(this) ;
    qSort(angles) ;
    angles << angles.first() + 360. ;
    qreal maxAngleGap = -1, result = 270 ;
    for (int i = 0 ; i < angles.size()-1 ; ++i)
    {
      qreal gap = angles[i+1] - angles[i] ;
      if (gap > maxAngleGap)
      {
        maxAngleGap = gap ;
        result = (angles[i+1]+angles[i]) / 2. ;
      }
    }
    return Molecule::toDegrees(result) ;
  }

  QVariant Atom::itemChange(GraphicsItemChange change, const QVariant &value)
  {
    if (change == ItemPositionChange && parentItem()) {
      parentItem()->update();
      dynamic_cast<Molecule*>(parentItem())->rebuild();
    };
    prepareGeometryChange();
    m_shape = computeBoundingRect();
    return graphicsItem::itemChange(change, value);
  }

  void Atom::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
  {
    if (event->button() != Qt::LeftButton) return;
    MolScene* sc = dynamic_cast<MolScene*>(scene());
    if(!sc || !sc->inputItem()) return;
    event->accept();
    TextInputItem *inputItem = sc->inputItem();
    sc->addItem(inputItem);
    inputItem->clickedOn(this);
  }

  void Atom::readGraphicAttributes(const QXmlStreamAttributes &attributes)
  {
    setElement(attributes.value("elementType").toString()) ;
  }

  QXmlStreamAttributes Atom::graphicAttributes() const
  {
    QXmlStreamAttributes attributes ;
    attributes.append("id", molecule()->atomId(this)) ;
    attributes.append("elementType", element()) ;
    attributes.append("hydrogenCount", QString::number(numImplicitHydrogens())) ;
    return attributes ;
  }


  //////////////////////////////////////////////////////////////////////////////
  // Event handlers
  //////////////////////////////////////////////////////////////////////////////

  void Atom::mousePressEvent( QGraphicsSceneMouseEvent* event )
  {
    // Execute default behavior
    graphicsItem::mousePressEvent( event );
  }

  void Atom::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
  {
    m_hidden = false;
    // Execute default behavior
    graphicsItem::hoverEnterEvent( event );
  }

  void Atom::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
  {
    m_hidden = true;
    // Execute default behavior
    graphicsItem::hoverLeaveEvent( event );
  }







  // Commands

  void Atom::setElement(const QString &element)
  {
    m_elementSymbol = element;
    prepareGeometryChange();
    m_shape = computeBoundingRect();
    molecule()->invalidateElectronSystems();
  }

  void Atom::setNumImplicitHydrogens(int number)
  {
    m_implicitHydrogens = true;

    m_userImplicitHydrogens = 0;
    int deltaH = number - numImplicitHydrogens();
//  int newNoB = m_numBonds - deltaNoIH;
  //m_numBonds = (newNoB < 0) ? 0 : newNoB;

    m_userImplicitHydrogens = deltaH;
  }


// Query methods

  int Atom::numBonds() const
  {
    return m_bonds.size();
  }

  int Atom::bondOrderSum() const
  {
    Molecule *mol = molecule();
    if (!mol) 
      return 0;

    // count explicit bonds
    int sum = 0;
    foreach (Bond *bond, mol->bonds(this))
      sum += bond->bondOrder();

    // take implicit hydrogens into account 
    sum += numImplicitHydrogens();

    return sum;
  }
      
  int Atom::numNonBondingElectrons() const
  {
    int boSum = bondOrderSum();
    switch (elementGroup(Element::strings.indexOf(m_elementSymbol))) {
      case 1:
      case 2:
      case 13:
      case 14:
        return 0 + m_userElectrons;
      case 15:
        if (boSum > 3)
          return 0 + m_userElectrons;
        else
          return 2 + 3 - boSum + m_userElectrons;
      case 16:
        switch (boSum) {
          case 0:
            return 6 + m_userElectrons;
          case 1:
            return 5 + m_userElectrons;
          case 2:
            return 4 + m_userElectrons;
          case 3:
            return 2 + m_userElectrons;
          default:
            return 0 + m_userElectrons;
        }
      case 17:
        if (boSum == 1)
          return 6 + m_userElectrons;
        else
          return 8 + m_userElectrons;
      case 18:
        return 8 + m_userElectrons;
      default:
        return 0 + m_userElectrons;
    }
  }

	QString Atom::string () const {
		QString el = element ();
		int n = numImplicitHydrogens();
		QString hs;
		QString num = "";
		if (n) {
			if (n > 1) num.setNum (n);
			hs = QString ("H") + num;
		}
		else hs = QString ("");
		QString q = chargeString();
		return el+hs+q;
	}
	
  int Atom::numImplicitHydrogens() const
  {
    Molecule *mol = molecule();
    if (!mol) 
      return 0;
    
    switch (Element::strings.indexOf(m_elementSymbol)) {
      case Element::B:
      case Element::C:
      case Element::N:
      case Element::O:
      case Element::P:
      case Element::S:
        break;
      default:
        return 0;
    }

    // count explicit bonds
    int bosum = 0;
    foreach (Bond *bond, mol->bonds(this))
      bosum += bond->bondOrder();

    int n = Molsketch::expectedValence(Molsketch::Element::strings.indexOf(m_elementSymbol)) - bosum + m_userImplicitHydrogens;
    
    return (n > 0) ? n : 0;
  }

  QString Atom::element() const
  {
    return m_elementSymbol;
  }

  int Atom::charge()  const
  {
    // non element atoms have no charge unless explicitly set (m_userCharge)
    int atomicNumber = Element::strings.indexOf(m_elementSymbol);
    if (!atomicNumber)
      return m_userCharge;
    
    // special case: He uses duet rule (<-> octet rule)
    if (atomicNumber == Element::He)
      return m_userCharge;
    
    return Molsketch::numValenceElectrons(atomicNumber) - bondOrderSum() - numNonBondingElectrons() + m_userCharge;
  }

  void Atom::setCharge(int requiredCharge)
  {
    int computedCharge = charge() - m_userCharge;
    m_userCharge = requiredCharge - computedCharge;
  }

  QString Atom::chargeString() const
  {
    // Get the charge
    int c = charge();

    // Drawing text
    QString string;
    string.setNum(c);
    if (c < -1) // ..., "3-", "2-"
      string =  string.remove(0,1) + "-";
    if (c == -1) // "-"
      string = "-";
    if (c == 0) // ""
      string = "";
    if (c == 1) // "+"
      string = "+";
    if (c > 1) // "2+", "3+", ...
      string = string + "+";

    // Return the charge string
    return string;
  }

  Molecule * Atom::molecule() const
  {
    return dynamic_cast<Molecule*>(this->parentItem());
  }

  void Atom::setMolecule(Molecule *molecule)
  {
    setParentItem(static_cast<QGraphicsItem*>(molecule));
  }

  bool Atom::hasImplicitHydrogens() const
  {
    return m_implicitHydrogens;
  }

  bool Atom::isDrawn() const
  {
    // If element is m_hidden, don't draw the atoms
    // Always draw the atom when there are no bonds
    if (!m_hidden || isSelected() || !numBonds()) return true;
    bool autoAddHydrogen = true;
    bool carbonVisible = false;
    bool chargeVisible = true;
    MolScene* molScene = dynamic_cast<MolScene*>(scene());
    if (molScene)
    {
      autoAddHydrogen = molScene->autoAddHydrogen();
      carbonVisible = molScene->carbonVisible();
      chargeVisible = molScene->chargeVisible();
    }

    if (m_elementSymbol == "H" && !autoAddHydrogen && (!chargeVisible)) // TODO charge() == 0 ?
          return false;
    if ((m_elementSymbol == "C") && !carbonVisible && (numBonds() > 1) && ((charge() == 0) || !chargeVisible))
      return false;
    return true;
  }

  bool Atom::isHidden() const
  {
    return m_hidden;
  }

  void Atom::setCoordinates(const QVector<QPointF> &c)
  {
    if (c.size() != 1) return ;
    setPos(c.first());
  }

  QPolygonF Atom::coordinates() const
  {
    return QVector<QPointF>() << pos() ;
  }

  void Atom::enableImplicitHydrogens(bool enabled)
  {
    m_implicitHydrogens = enabled/* && (m_elementSymbol == "C" || m_elementSymbol == "N" || m_elementSymbol == "O")*/;
  }

  void Atom::addBond(Bond *bond)
  {
    if (!bond) return;

    if (!m_bonds.contains(bond))
      m_bonds.append(bond);
    
    prepareGeometryChange();
    m_shape = computeBoundingRect();
  }

  void Atom::removeBond(Bond *bond)
  {
    m_bonds.removeAll(bond);
    prepareGeometryChange();
    m_shape = computeBoundingRect();
  }

  QList<Bond*> Atom::bonds() const
  {
    return m_bonds;
  }


  QList<Atom*> Atom::neighbours() const
  {
    QList<Atom*> nbrs;
    foreach (Bond *bond, m_bonds)
      nbrs.append(bond->otherAtom(this));
    return nbrs;
  }

} // namespace
