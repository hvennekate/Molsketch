/***************************************************************************
 *   Copyright (C) 2007 by Harm van Eersel                                 *
 *   devsciurus@xs4all.nl                                                  *
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

/** @file
 * This file is part of molsKetch and contains the bond class.
 *
 * @author Harm van Eersel <devsciurus@xs4all.nl>
 * @since Hydrogen
 */

#ifndef BOND_H
#define BOND_H

#include "atom.h"
#include "graphicsitem.h"


namespace Molsketch {

  /**
 * Represents a bond.
 *
 * @author Harm van Eersel
 */
  class Bond : public graphicsItem
  {
    friend class Molecule;

  public:
    enum { Type = graphicsItem::BondType };
    int type() const override { return Type; }

    enum BondType
    {
      Invalid = 0,
      DativeDot = 1,
      DativeDash = 2,
      Single = 10,
      Wedge = 11,
      Hash = 12,
      WedgeOrHash = 13,
      Thick = 14,
      Striped = 15,
      DoubleLegacy = 20,
      CisOrTrans = 21,
      DoubleAsymmetric = 22,
      DoubleSymmetric = 23,
      Triple = 30,
      TripleAsymmetric = 31, // TODO more?
    };

    static int orderFromType(const BondType& type);
    static BondType simpleTypeFromOrder(const int& order);

    /**
         * Constructor. Create a new bond between @p atomA and @p atomB.
         *
         * @param atomA the origin atom of the bond
         * @param atomB the target atom of the bond
         * @param order the bond order (@c Bond::Single for single, @c Bond::Double for double, @c Bond::Triple for tripple)
         * @param type the bond type (@c Bond::Normal, @c Bond::Up, @c Bond::Down, e.g.)
         */
    explicit Bond(Atom* atomA = 0, // TODO check usage
                  Atom* atomB = 0,
                  Bond::BondType type = Single,
                  QGraphicsItem* parent = 0) ;
    explicit Bond(const Bond& other, Atom *atomA = 0, Atom *atomB = 0);

    virtual ~Bond();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant & value) override;

    virtual QPainterPath shape() const override;
    virtual QRectF boundingRect() const override;
    /** Returns the angle of this bond from atom @p origin */
    qreal bondAngle(const Atom* origin) const ;
    /** Returns the main bond axis */
    QLineF bondAxis() const ;

    void setType(const Bond::BondType& type);
    int bondOrder() const;
    Bond::BondType bondType() const;
    Atom* beginAtom() const;
    Atom* endAtom() const;
    bool hasAtom(const Atom* atom) const;

    Atom* otherAtom(const Atom *atom) const;
    void setAtoms(Atom* A, Atom* B) ;
    void setAtoms(const QPair<Atom*, Atom*>& atoms);
    QPair<Atom*, Atom*> atoms() const;

    Molecule* molecule() const;

    /**
         * Auxillary method for shifting a bond perpendicular to the original bond.
         * Needed for the drawing of multiple bonds.
         *
         * @param vector the original vector that is to be shifted
         * @param shift the amount of shifting
         *
         * @return the shifted vector
         */
    static QLineF shiftVector(const QLineF & vector, qreal shift);

    QString xmlName() const override;
    static QString xmlClassName();
    /** set the coordinates of the two atoms */
    void setCoordinates(const QVector<QPointF> &c) override;
    /** get the coordinates of the two atoms */
    QPolygonF coordinates() const override;

  protected:
    QXmlStreamAttributes graphicAttributes() const override;
    void readGraphicAttributes(const QXmlStreamAttributes &attributes) override;
    void prepareContextMenu(QMenu *contextMenu) override;
    XmlObjectInterface* produceChild(const QString &name, const QXmlStreamAttributes &attributes) override;
    void afterReadFinalization() override;
    virtual QPainterPath outline() const;

  private:
    QPainterPath drawHashBond() const;
    QPainterPath drawWedgeBond() const;
    QPainterPath drawWavyBond() const;
    QPainterPath drawThickBond() const;
    QPainterPath drawStripedBond() const;
    QPainterPath getWedgeBondShape() const;
    QPainterPath getThickBondShape() const;
    void determineDoubleBondOrientation();
    QPainterPath bondPath() const;
    QPointF determineBondDrawingStart(Atom* start, Atom* end) const;
    QList<Bond*> coveringBonds() const;

    // Internal representation
    Bond::BondType m_bondType;
    Atom* m_beginAtom;
    Atom* m_endAtom;
    QList<XmlObjectInterface*> helpers;
    QLineF mapOuterLineToAtom(const Atom *atom, const QLineF &line, bool reverse) const;
    qreal getExtentForStereoBond(const Atom *atom, const QPair<QLineF, QLineF> &outerLines, bool reverse) const;
    QPair<QLineF, QLineF> getOuterLimitsOfStereoBond() const;
    QPair<QLineF, QLineF> getOuterLimitsOfThickBond() const;
    qreal bondShapeGap() const;
    void paintBrokenBondIndicators(QPainter *painter, const QPointF &begin, const QPointF &end, const QPointF &vb, const QPointF &normalVector);
    QPainterPath getBrokenBondIndicatorsPath(const QPointF &begin, const QPointF &end, const QPointF &normalVector) const;
    QPainterPath clipBrokenBondIndicator(const QPointF &point, const QPointF &otherAtom, const QPointF &normalVector) const;
    QPainterPath getBondShapeFromOuterLines(const QPair<QLineF, QLineF> &outerLines) const;
    QPainterPath drawBondComprisedOfStripes(const QPair<QLineF, QLineF> &outerLines) const;
  };

} // namespace

#endif
