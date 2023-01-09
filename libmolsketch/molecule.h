/***************************************************************************
 *   Copyright (C) 2007 by Harm van Eersel                                 *
 *   Copyright (C) 2009 by Tim Vandermeersch                               *
 *   Copyright (C) 2018 by Hendrik Vennekate                               *
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
* This file is part of molsKetch and defines the class Molecule.
*
* @author Harm van Eersel <devsciurus@xs4all.nl>
*/


#ifndef MOLECULE_H
#define MOLECULE_H

#include "graphicsitem.h"
#include "abstractxmlobject.h"
#include "bond.h"

#include <QList>
#include <QGraphicsItemGroup>
#include "moleculepopup.h"
#include "core/coremolecule.h"

class QString;
class QPoint;
class QPainter;

namespace Molsketch {

  class Atom;
  class MolScene;
  class ElectronSystem; // under construction
  class MoleculePrivate;

 /**
 * Represents a molecule on the scene. It can be created either as an empty molecule,
 * with a set of atoms and bonds or as a copy of another molecule.
 *
 * @author Harm van Eersel <devsciurus@xs4all.nl>
 * @since Hydrogen
 */
  class Molecule : public graphicsItem
  {
    Q_DECLARE_PRIVATE (Molecule)
    QScopedPointer<MoleculePrivate> d_ptr;
  public:
    enum { Type = graphicsItem::MoleculeType };
    int type() const override { return Type; }

    static qreal toDegrees(const qreal& angle) ;

    explicit Molecule(QGraphicsItem* parent = 0) ;
    Molecule(QSet<Atom*>, QSet<Bond*>, QGraphicsItem* parent = 0) ;
    // TODO get bonds from atoms or atoms from bonds, but don't take both
    /** Creates a copy of molecule @p mol with @p parent on MolScene @p scene. */
    explicit Molecule(const Molecule& mol) ;

    explicit Molecule(const Core::Molecule &coreMolecule, qreal scaling = 1, QGraphicsItem *parent = 0);
    static Molecule *fromCoreMolecule(const Core::Molecule &coreMolecule, qreal scaling = 1);

    /** Creates a copy of molecule @p mol, but clones @p atoms as the new molecule's
         * atoms, and any bonds where both atoms  are in @p atoms. */
    Molecule(const Molecule& mol, const QSet<Atom*>& atoms, QGraphicsItem* parent = 0);
    // TODO constructor with initializer block

    ~Molecule();

    static Molecule *combineMolecules(const QSet<Molecule *> &molecules, QMap<Atom*, Atom*>*atomMap, QMap<Bond *, Bond *> *bondMap);

    QRectF boundingRect() const override;

    /** Paint method to draw the atom onto a QPainter. Needed for Qt painting.*/
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;


    /** Coordinates */
    QPolygonF coordinates() const override;
    /** Set coordinates */
    void setCoordinates(const QVector<QPointF> &c) override;

    Atom* addAtom(Atom* atom); // TODO remove

    /**
        * This method removes an atom from the molecule. The bonds connected to this atom will be removed as well.
        * The atom and bonds will be removed from the scene, but the ownership will be passed on to the caller. This
        * can be neccecary for undo operations.
        *
        * @param atom a pointer to the atom that should be removed from the molecule.
        *
        * @return a list with the bonds which were connected to the removed atom..
        */
    QList<Bond*> delAtom(Atom* atom);
    /**
        * Returns the "center of mass", i.e. the average of all atom coordinates
        * (no actual mass involved)
        */
    QPointF graphicalCenterOfMass() const ;


    /** Adds a bond between @p atomA and @p atomB with @p order and @p type. */
    Bond* addBond(Atom* atomA, Atom* atomB, Bond::BondType type = Bond::Single, QColor c = QColor (0, 0, 0));
    /** Adds existing bond @p bond to the molecule. */
    Bond* addBond(Bond* bond);
    /** Deletes @p bond from the molecule. */
    void delBond(Bond* bond);

    void updateTooltip();

    /**
          * Splits the molecule up in different seperate molecules. Used to clean up the molecule after removing the connection
        * between two or more parts of the molecule.
        *
        * @return a list of the submolecules of which this molecule exists.
        */
    QList<Molecule*> split();


    /**
        * This method rebuilds the atom by removing all atoms and bonds from the molecule and consequently readding them.
        * This is a hack to reset the boundingbox of the molecule after moving one of the atoms and is this method is in need
        * of a proper solution.
        */
    void rebuild();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /// Returns the pointer from ID
    Atom* atom(const QString& atomID) const ;

    /// Returns a list of the bonds connected to @p atom.
    QList<Bond*> bonds(const Atom* atom);
    /// Returns a pointer to the bond between @p atomA and @p atomB, or a NULL if none.
    Bond* bondBetween(const Atom *atomA, const Atom *atomB) const;

    /// @return @c true if the molecule exists of two seperate submolecules, and @c false otherwise.
    bool canSplit() const;

    QList<Atom*> atoms() const; // TODO return set
    QList<Bond*> bonds() const; // TODO return set
    SumFormula sumFormula() const;

    QWidget *getPropertiesWidget() override;

    /** Returns the MolScene of the molecule. */
    virtual MolScene* scene() const;

    /** Returns the molecule formula. */
    QString formula() const;

    /**
      * Invalidate the electron systems. To be called when Atom/Bond properties
      * change.
      */
    void invalidateElectronSystems();

    QString xmlName() const override;
    static QString xmlClassName();

  protected:

    void prepareContextMenu(QMenu *contextMenu) override;

    /** Event handler for changes of the molecule. Needed for rotation handling.*/
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QList<const XmlObjectInterface *> children() const override;
    XmlObjectInterface *produceChild(const QString &name, const QXmlStreamAttributes &attributes) override;
    void afterReadFinalization() override;
    void readAttributes(const QXmlStreamAttributes& attributes) override;
    QXmlStreamAttributes xmlAttributes() const override;
  private:
    void redoIndexes();
    void setDefaults();
    void clone(QSet<Atom*> atoms);
    QList<Atom*> smallestRing(QList<Atom*> atomList) const ;
    QString name;
    void paintElectronSystems(QPainter *painter) const;
    void collectElectronSystems();
    void mergeElectronSystems();
  public:
    bool m_electronSystemsUpdate; // TODO remove
    QList<ElectronSystem*> m_electronSystems;
    void updateElectronSystems();

    QString getName() const;
    void setName(const QString &value);
    Core::Molecule toCoreMolecule(qreal scaling = 1) const;
  };

} // namespace

#endif
