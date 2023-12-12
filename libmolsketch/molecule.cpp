/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2009 Tim Vandermeersch                                  *
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


#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include <QtGui>

#include <actions/frametypeaction.h>
#include <actions/flipstereobondsaction.h>

#include "molecule.h"

#include "qtdeprecations.h"

#include "atom.h"
#include "bond.h"
#include "molscene.h"
#include "element.h"
#include "math2d.h"

#include "electronsystem.h"
#include "scenesettings.h"
#include "settingsitem.h"
#include "core/coreatom.h"
#include "core/corebond.h"

namespace Molsketch {

  class AtomProxyList : public abstractXmlObject {
    Molecule* p ;
  public:
    explicit AtomProxyList(Molecule* parent) : p(parent) {}
    QString xmlName() const override { return "atomArray"; }
    QList<const XmlObjectInterface *> children() const override {
      QList<const XmlObjectInterface*> result;
      for (auto atom :  p->atoms()) result << atom;
      return result;
    }
    abstractXmlObject *produceChild(const QString &name, const QXmlStreamAttributes &attributes) override {
      Q_UNUSED(attributes)
      if (name != Atom::xmlClassName()) return nullptr;
      Atom *atom = attributes.hasAttribute("hydrogenCount") ? new LegacyAtom() : new Atom();
      atom->setParentItem(p);
      return atom;
    }
  };

  class BondProxyList : public abstractXmlObject {
    Molecule* p ;
  public:
    explicit BondProxyList(Molecule* parent) : p(parent) {}
    QString xmlName() const override { return "bondArray"; }
    QList<const XmlObjectInterface *> children() const override {
      QList<const XmlObjectInterface*> result;
      for (auto bond :  p->bonds()) result << bond;
      return result;
    }
    abstractXmlObject *produceChild(const QString &name, const QXmlStreamAttributes &attributes) override {
      Q_UNUSED(attributes)
      if (name != Bond::xmlClassName()) return nullptr;
      auto bond = new Bond();
      bond->setParentItem(p);
      return bond;
    }
  };

  struct MoleculePrivate {
    AtomProxyList atomList;
    BondProxyList bondList;
    explicit MoleculePrivate(Molecule* m) : atomList(m), bondList(m) {}
  };

  // Constructors

  qreal Molecule::toDegrees(const qreal &angle)
  {
    qreal temp = (angle - (int) angle) + ((int)angle % 360) ;
    return temp < 0 ? 360 + temp : temp ;
  }

#define DEFAULTINITIALIZER d_ptr(new MoleculePrivate(this)), m_electronSystemsUpdate(true)

  Molecule::Molecule(QGraphicsItem* parent)
    : graphicsItem(parent),
      DEFAULTINITIALIZER
  {
    setDefaults();
  }

  Molecule::Molecule(QSet<Atom*> atomSet, QSet<Bond*> bondSet,
                     QGraphicsItem* parent)
    : graphicsItem (parent),
      DEFAULTINITIALIZER
  {
    setDefaults();

    foreach(Atom* atom, atomSet) addAtom(atom);

    // ...and bonds
    foreach(Bond* bond, bondSet)
    {
      addBond(bond);
      Atom *begin = bond->beginAtom(), *end = bond->endAtom();
      if (begin && !atomSet.contains(begin)) addAtom(begin);
      if (end && !atomSet.contains(end)) addAtom(end);
    }
  }

  Molecule::Molecule(const Molecule &mol)
    : graphicsItem (mol),
      DEFAULTINITIALIZER
  {
    setDefaults();
    auto atoms = mol.atoms();
    auto atomSet = toSet(atoms);
    clone(atomSet);
    setPos(mol.pos());
    updateElectronSystems();
    updateTooltip();
  }

  Bond::BondType fromCoreBondType(const Core::Bond::Type &type) {
    switch (type) {
      case Core::Bond::Invalid: return Bond::Invalid;
      case Core::Bond::DativeDot: return Bond::DativeDot;
      case Core::Bond::DativeDash : return Bond::DativeDash;
      case Core::Bond::Single : return Bond::Single;
      case Core::Bond::Wedge : return Bond::Wedge;
      case Core::Bond::Hash : return Bond::Hash;
      case Core::Bond::WedgeOrHash : return Bond::WedgeOrHash;
      case Core::Bond::Thick : return Bond::Thick;
      case Core::Bond::Striped : return Bond::Striped;
      case Core::Bond::DoubleLegacy : return Bond::DoubleLegacy;
      case Core::Bond::CisOrTrans : return Bond::CisOrTrans;
      case Core::Bond::DoubleAsymmetric : return Bond::DoubleAsymmetric;
      case Core::Bond::DoubleSymmetric : return Bond::DoubleSymmetric;
      case Core::Bond::Triple : return Bond::Triple;
      case Core::Bond::TripleAsymmetric : return Bond::TripleAsymmetric;
    }
    return Bond::Invalid;
  }

  Molecule::Molecule(const Core::Molecule &coreMolecule, qreal scaling, QGraphicsItem *parent)
    : graphicsItem(parent),
      DEFAULTINITIALIZER
  {
    setName(coreMolecule.name());

    QList<Atom*> newAtoms;
    QMap<Atom*, QPair<unsigned, int>> hAtomsAndCharges;
    for (auto atom : coreMolecule.atoms()) {
      auto newAtom = new Atom(atom.position() * scaling, atom.element());
      hAtomsAndCharges[newAtom] = qMakePair(atom.hAtoms(), atom.charge());
      newAtoms << newAtom;
      addAtom(newAtom);
    }
    for (auto bond : coreMolecule.bonds())
      if (auto start = newAtoms.value(bond.start()))
        if(auto end = newAtoms.value(bond.end()))
          addBond(new Bond(start, end, fromCoreBondType(bond.type())));
    for (auto it = hAtomsAndCharges.cbegin(); it != hAtomsAndCharges.cend(); ++it) {
      it.key()->setNumImplicitHydrogens(it.value().first);
      it.key()->setCharge(it.value().second);
    }
  }

  Molecule *Molecule::fromCoreMolecule(const Core::Molecule &coreMolecule, qreal scaling) {
    return coreMolecule.isValid() ? new Molecule(coreMolecule, scaling) : nullptr;
  }

  Molecule::Molecule(const Molecule &mol, const QSet<Atom *> &atoms, QGraphicsItem *parent)
    : graphicsItem (mol),
      DEFAULTINITIALIZER
  {
    setParentItem(parent);
    setDefaults();
    clone(atoms);
    setPos(mol.pos()); // TODO check this
  }

  Molecule::~Molecule() {}

  void Molecule::clone(QSet<Atom *> atoms)
  {
    QMap<Atom*, Atom*> oldToNewAtoms;
    QSet<Bond*> oldBonds;
    foreach(Atom* atom, atoms)
    {
      Atom *newAtom = new Atom(*atom);
      oldToNewAtoms[atom] = newAtom;
      addAtom(newAtom);
      foreach(Bond* bond, atom->bonds())
        if (atoms.contains(bond->beginAtom())
            && atoms.contains(bond->endAtom()))
          oldBonds << bond;

    }

    foreach(Bond* bond, oldBonds)
      addBond(new Bond(*bond, oldToNewAtoms[bond->beginAtom()],
              oldToNewAtoms[bond->endAtom()]));
  }

  Molecule* Molecule::combineMolecules(const QSet<Molecule*>& molecules, QMap<Atom *, Atom *> *givenAtomMap, QMap<Bond*,Bond*> *givenBondMap)
  {
    Molecule* result = new Molecule;
    QMap<Atom*,Atom*> atomMap;
    QMap<Bond*,Bond*> bondMap;
    foreach(Molecule* molecule, molecules)
    {
      if (!molecule) continue;
      foreach(Atom* atom, molecule->atoms())
      {
        Atom* newAtom = new Atom(*atom);
        result->addAtom(newAtom);
        atomMap[atom] = newAtom;
      }
      foreach(Bond* bond, molecule->bonds())
      {
        Bond* newBond = new Bond(*bond, atomMap[bond->beginAtom()], atomMap[bond->endAtom()]);
        result->addBond(newBond);
        bondMap[bond] = newBond;
      }
    }
    if (givenAtomMap) atomMap.swap(*givenAtomMap);
    if (givenBondMap) bondMap.swap(*givenBondMap);
    return result;
  }

  void Molecule::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    event->ignore();
    for (auto item : scene()->items(event->scenePos()))
      if (dynamic_cast<Atom*>(item) || dynamic_cast<Bond*>(item)) return;

    graphicsItem::mousePressEvent(event);
  }

  Atom* Molecule::addAtom(Atom* atom)
  {
    if (!atom) return atom;
    atom->setParentItem(this);
    m_electronSystemsUpdate = true;
    redoIndexes();
    updateTooltip();
    return atom;
  }

  Bond* Molecule::addBond(Atom* atomA, Atom* atomB, Bond::BondType type, QColor c) // TODO remove
  {
    if (atomA == atomB) return 0;

    Bond* bond = new Bond(atomA,atomB,static_cast<Bond::BondType>(type));
    bond ->setColor(c);
    return addBond(bond);
  }

  Bond* Molecule::addBond(Bond* bond)
  {
    Q_CHECK_PTR(bond);

    if (scene ()) bond->setColor(scene()->settings()->defaultColor()->get());
    // Checking if and altering when a bond exists
    Bond* bondX = bondBetween(bond->beginAtom(), bond->endAtom());
    if (bondX) {
      delete bond;
      if (scene()) bondX->setColor(scene()->settings()->defaultColor()->get());
      return bondX;
    }

    bond->setParentItem(this);

    bond->setAtoms(bond->beginAtom(), bond->endAtom()); // updates pos and atom shapes

    m_electronSystemsUpdate = true;
    updateTooltip();
    return bond;
  }

  QList<Bond*> Molecule::delAtom(Atom* atom)
  {
    QList<Bond*> delList = bonds(atom);
    foreach(Bond* bond, delList) {
      bond->setParentItem(0);
      if (scene()) scene()->removeItem(bond);
    }

    atom->setParentItem(0);
    if (scene()) scene()->removeItem(atom);

    m_electronSystemsUpdate = true;
    redoIndexes();
    updateTooltip();

    return delList;
  }

  QPointF Molecule::graphicalCenterOfMass() const
  {
    QPointF com ;
    auto atomList = atoms();
    foreach(Atom* atom, atomList)
      com += atom->pos() ;
    return com / atomList.size() ;
  }

  void Molecule::delBond(Bond* bond)
  {
    if (!bond || bond->parentItem() != this) return;

    bond->setParentItem(0);
    if (scene()) scene()->removeItem(bond);

    m_electronSystemsUpdate = true;
    updateTooltip();
  }

  void Molecule::updateTooltip() {
    setToolTip(sumFormula().toHtml());
  }

  QSet<Atom*> getConnectedAtoms(Atom* startAtom)
  {
    QSet<Atom*> connectedAtoms = {startAtom};
    int lastSize = 0;
    while (lastSize < connectedAtoms.size())
    {
      lastSize = connectedAtoms.size();
      for(auto atom : connectedAtoms)
        for (auto neighbor : atom->neighbours())
          connectedAtoms += neighbor;
    }
    return connectedAtoms;
  }

  QList<Molecule*> Molecule::split()
  {
    QList<Molecule*> molList;

    auto atomList = atoms();
    auto atomSet = toSet(atomList);
    while (!atomSet.empty())
    {
      QSet<Atom*> subgroup = getConnectedAtoms(*(atomSet.begin()));
      molList << new Molecule(*this, subgroup);
      atomSet -= subgroup;
    }

    return molList;
  }

  Atom *Molecule::atom(const QString &atomID) const {
    for (auto atom : atoms()) if (atom->index() == atomID) return atom;
    return nullptr;
  }

  QList<Atom*> Molecule::atoms() const {
    return childrenByType<Atom*>();
  }

  QList<Bond*> Molecule::bonds() const {
    return childrenByType<Bond*>();
  }

  SumFormula Molecule::sumFormula() const {
    SumFormula result;
    for (auto atom : atoms()) result += atom->sumFormula();
    return result;
  }

  QWidget *Molecule::getPropertiesWidget()
  {
    MoleculePopup* widget = new MoleculePopup;
    widget->connectMolecule(this);
    return widget;
  }

  QString Molecule::formula( ) const
  {
    //pre: true
    //ret: formula of the molecule

    // Create the output object
    QString formula;

    // Analyse the molecule and create a hash with the atoms
    QHash<QString,int> hash;

    //     for (int i = 0; i < countAtoms(); i++)
    foreach(Atom* atom, atoms())
    {
      QString element = atom->element();
      hash.insert(element, hash.value(element,0) + 1 );

      // Add implicit hydrogens?
      int hydrogens = atom->numImplicitHydrogens();
      if ( hydrogens > 0 ) hash.insert("H", hash.value("H",0) + hydrogens );
    }

    // First Carbon
    if (hash.contains("C"))
      formula += "C" + (hash.value("C") > 1?formula.number(hash.value("C")):"");

    // Then Nitrogen
    if (hash.contains("N"))
      formula += "N" + (hash.value("N") > 1?formula.number(hash.value("N")):"");

    // Compose the formula form the hash
    QHashIterator<QString, int> i(hash);

    // Then other elements
    while (i.hasNext())
    {
      i.next();
      if (!(i.key() == "C" || i.key() ==  "N"  || i.key() ==  "H") )
      {
        formula += i.key() + (i.value() > 1?formula.number(i.value()):"" );
      }
    }

    // Finally Hydrogen
    if (hash.contains("H"))
      formula += "H" + (hash.value("H") > 1?formula.number(hash.value("H")):"");

    // Return the formula
    return formula;
  }

  bool Molecule::canSplit() const
  {
    if (atoms().isEmpty()) return false;
    auto atomList = atoms();
    auto atomSet = toSet(atomList);
    return getConnectedAtoms(atoms().first()) != atomSet;
  }

  QVariant Molecule::itemChange(GraphicsItemChange change, const QVariant &value)
  {
    if (change == ItemTransformHasChanged) rebuild();
    if (change == ItemChildAddedChange || change == ItemChildRemovedChange) {
      redoIndexes();
      updateTooltip();
    }

    return graphicsItem::itemChange(change, value);
  }

  void Molecule::rebuild()
  {
    //pre: true
    //post: the molecule has been rebuild

    // Remove and then readd all elements
    prepareGeometryChange();

    update();

  }


  Bond* Molecule::bondBetween(const Atom* atomA, const Atom* atomB) const
  {
    //     for (int i = 0; i < countBonds(); i++)
    foreach(Bond* bond, bonds())
      if (((bond->beginAtom() == atomA) || (bond->beginAtom() == atomB )) && ((bond->endAtom() == atomA) || (bond->endAtom() == atomB )))
        return bond;

    return 0;
  }

  QList< Bond * > Molecule::bonds(const Atom * atom)
  {
    QList<Bond*> bondList;

    foreach(Bond* bond, bonds())
      if (bond->hasAtom(atom))
        bondList << bond;

    return bondList;
  }

void Molecule::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)

  // draw a blue rectangle if this molecule is selected
  if(isSelected()) {
    painter->save();
    painter->setPen(Qt::blue);
    painter->drawRect(boundingRect()); // TODO simply draw rectangle around own and children!
    painter->restore();
  } // TODO move this somewhere else, so that it can have a different Z value

  // draw the electron systems

  if (!scene()) return;
  if (scene()->settings()->electronSystemsVisible()->get()) {
    updateElectronSystems();
    paintElectronSystems(painter);
  }

  graphicsItem::paint(painter, option, widget);
}

void Molecule::paintElectronSystems(QPainter *painter) const {
  QPen pen = painter->pen();
  pen.setWidth(10);
  pen.setCapStyle(Qt::RoundCap);
  pen.setColor(QColor(255, 200, 0));
  painter->setPen(pen);
  //  painter->setOpacity(0.3);

  QList<Atom*> doneList;

  foreach (ElectronSystem *es, m_electronSystems) {
    QPointF midPoint(0.0, 0.0);

    if (es->atoms().size() == 1) {
      QPointF dir(0.0, 0.0), orthogonal;
      Atom *atom = es->atoms().at(0);
      doneList.append(atom);
      foreach (Atom *nbr, atom->neighbours())
        dir += normalized(nbr->pos() - atom->pos());
      dir /= atom->numBonds();
      dir = normalized(dir);
      if (!atom->numBonds()) {
        dir = QPointF(0.0, 1.0);
        orthogonal = QPointF(1.0, 0.0);
      } else {
        orthogonal = QPointF(dir.y(), -dir.x());
      }
      switch (doneList.count(atom)) {
        case 1:
          switch (atom->numBonds()) {
            case 1:
              painter->drawEllipse(atom->scenePos() - 15 * orthogonal, 5, 5);
              break;
            default:
              painter->drawEllipse(atom->scenePos() - 15 * dir, 5, 5);
              break;
          }
          break;
        case 2:
          switch (atom->numBonds()) {
            case 1:
              painter->drawEllipse(atom->scenePos() + 15 * orthogonal, 5, 5);
              break;
            default:
              painter->drawEllipse(atom->scenePos() + 15 * dir, 5, 5);
              break;
          }
          break;
        case 3:
          switch (atom->numBonds()) {
            case 1:
              painter->drawEllipse(atom->scenePos() - 15 * dir, 5, 5);
              break;
            default:
              painter->drawEllipse(atom->scenePos() - 15 * orthogonal, 5, 5);
              break;
          }
          break;
        case 4:
          painter->drawEllipse(atom->scenePos() - 15 * orthogonal, 5, 5);
          break;
        default:
          painter->drawEllipse(atom->scenePos(), 5, 5);
          break;

      }

    } else {
      foreach (Atom *a, es->atoms()) {
        foreach (Atom *b, es->atoms()) {
          if (bondBetween(a, b))
            painter->drawLine(a->scenePos(), b->scenePos());
          //          painter->drawEllipse(atom->scenePos(), 5, 5);
        }

        midPoint += mapToParent(a->scenePos());
      }
    }

    if (es->numAtoms() < 2)
      continue;
    midPoint /= es->numAtoms();

    painter->save();
    painter->setPen(Qt::black);
    QPointF offset(20.0, 20.0);
    painter->drawText(QRectF(midPoint - offset, midPoint + offset), Qt::AlignCenter, QString("%1pi").arg(es->numElectrons()));
    painter->restore();


  }
}

  QPolygonF Molecule::coordinates() const // TODO this should be removed when introducing relative coords
  {
    QVector<QPointF> result ;
    foreach (Atom* atom, atoms())
      result << atom->coordinates() ;
    return result ;
  }

  void Molecule::setCoordinates(const QVector<QPointF> &c)
  {
    if (c.size() != atoms().size()) return ;
    for (int i = 0 ; i < c.size() ; ++i)
      atoms()[i]->setCoordinates(c.mid(i,1)) ;
  }

  QRectF Molecule::boundingRect() const
  {
    return childrenBoundingRect();
  }

  MolScene* Molecule::scene() const
  {
    return static_cast<MolScene*>(QGraphicsItem::scene());
  }

  bool NumAtomsMoreThan(const ElectronSystem *es1, const ElectronSystem *es2)
  {
    return es1->numAtoms() > es2->numAtoms();
  }

bool canMerge(const ElectronSystem *es1, const ElectronSystem *es2)
{
  auto firstListOfAtoms = es1->atoms();
  auto firstSetOfAtoms = toSet(firstListOfAtoms);
  auto secondListOfAtoms = es2->atoms();
  auto secondSetOfAtoms = toSet(secondListOfAtoms); // TODO utility function
  // may not share an atom
  if (!(firstSetOfAtoms & secondSetOfAtoms).empty()) return false;

  for (auto atomFromFirstSet : firstSetOfAtoms)
    for (auto neighborOfAtomFromFirstSet : atomFromFirstSet->neighbours())
      if (secondSetOfAtoms.contains(neighborOfAtomFromFirstSet))
        return true;
  return false;
}

void merge(QList<ElectronSystem*> &electronSystems, ElectronSystem *es1, ElectronSystem *es2)
{
  es1->setAtoms(es1->atoms() + es2->atoms());
  es1->setNumElectrons(es1->numElectrons() + es2->numElectrons());
  electronSystems.removeAll(es2);
  delete es2;
}

void Molecule::invalidateElectronSystems()
{
  m_electronSystemsUpdate = true;
}

QString Molecule::xmlName() const { return xmlClassName(); }

QString Molecule::xmlClassName() { return "molecule" ; }

// TODO make default function that calls this one instead
void Molecule::prepareContextMenu(QMenu *contextMenu)
{
  MolScene *sc = qobject_cast<MolScene*>(scene());
  if (sc)
  {
    auto action = sc->findChild<FrameTypeAction*>();
    if (action)
    {
      contextMenu->addAction(action);
      QObject::connect(action, SIGNAL(triggered()), contextMenu, SLOT(close()));
    }
    flipStereoBondsAction* flipStereo = sc->findChild<flipStereoBondsAction*>();
    if (flipStereo)
    {
      contextMenu->addAction(flipStereo); // TODO test with bond AND molecule selected simultaneously
      QObject::connect(flipStereo, SIGNAL(triggered()), contextMenu, SLOT(close()));
    }
  }
  graphicsItem::prepareContextMenu(contextMenu);
}

void Molecule::collectElectronSystems() {
  foreach (ElectronSystem *es, m_electronSystems)
    delete es;
  m_electronSystems.clear();

  foreach (Bond *bond, bonds()) {
    unsigned int piOrder = qMax(bond->bondOrder() - 1, 0);
    while (piOrder--) m_electronSystems << new PiElectrons(bond->atoms(), 2);
  }

  foreach (Atom *atom, atoms()) {
    unsigned int unboundElectronPairs = qMax(atom->numNonBondingElectrons() / 2, 0);
    while (unboundElectronPairs--) m_electronSystems << new PiElectrons({atom}, 2);
    if (atom->numNonBondingElectrons() % 2) m_electronSystems << new PiElectrons({atom}, 1);
  }
  std::sort(m_electronSystems.begin(), m_electronSystems.end(), NumAtomsMoreThan); // TODO this should be redundant
}

void Molecule::mergeElectronSystems() {
  bool restart = true;
  while (restart) {
    restart = false;
    foreach (ElectronSystem *es1, m_electronSystems) {
      foreach (ElectronSystem *es2, m_electronSystems) {
        if (canMerge(es1, es2)) {
          merge(m_electronSystems, es1, es2);
          restart = true;
          break;
        }
      }

      if (restart)
        break;
    }
  }
}

void Molecule::updateElectronSystems()
{
  if (!m_electronSystemsUpdate)
    return;
  m_electronSystemsUpdate = false;

  collectElectronSystems();
  mergeElectronSystems();
}

  QList<const XmlObjectInterface *> Molecule::children() const
  {
    Q_D(const Molecule);
    return QList<const XmlObjectInterface*>() << &d->atomList << &d->bondList ;
  }

  XmlObjectInterface *Molecule::produceChild(const QString &name, const QXmlStreamAttributes &attributes) {
    Q_UNUSED(attributes)
    Q_D(Molecule);
    if (d->atomList.xmlName() == name) return &d->atomList;
    if (d->bondList.xmlName() == name) return &d->bondList;
    return nullptr;
  }

  void Molecule::afterReadFinalization()
  {
    for (auto atom : atoms()) atom->afterMoleculeReadFinalization();
    updateElectronSystems();
    updateTooltip();
  }

  void Molecule::readAttributes(const QXmlStreamAttributes &attributes)
  {
    name = attributes.value("name").toString();
  }

  QXmlStreamAttributes Molecule::xmlAttributes() const
  {
    QXmlStreamAttributes attributes;
    attributes.append("name", name);
    return attributes;
  }

  void Molecule::redoIndexes() {
    int i = 0;
    for (auto atom : atoms())
      atom->setIndex(QString("a") + QString::number(++i));
  }

  void Molecule::setDefaults()
  {
    setHandlesChildEvents(false);
    setAcceptHoverEvents(true) ;
    setZValue(-50);
  }

  QList<Atom*> Molecule::smallestRing(QList<Atom*> atomList) const // TODO test
  { // TODO JAVA-style iterators for clarity
    if (atomList.empty()) return atomList ;
    int size = INT_MAX ;
    QList<Atom*> ring ;
    foreach (Atom* next, atomList.last()->neighbours())
    {
      if (atomList.size() > 2 && atomList.first() == next) return atomList ;
      if (atomList.contains(next)) continue ;
      QList<Atom*> minRing = smallestRing(QList<Atom*>(atomList) << next) ;
      if (minRing.size() < size && !minRing.empty())
      {
        size = minRing.size() ;
        ring = minRing ;
      }
    }
    return ring ;
  }
  QString Molecule::getName() const
  {
    return name;
  }

  void Molecule::setName(const QString &value)
  {
    name = value;
  }

  /* This is to let the compiler check that we properly convert all bond types */
  Core::Bond::Type toCoreBondType(const Bond::BondType &type) {
    switch (type) {
      case Bond::Invalid: return Core::Bond::Invalid;
      case Bond::DativeDot: return Core::Bond::DativeDot;
      case Bond::DativeDash : return Core::Bond::DativeDash;
      case Bond::Single : return Core::Bond::Single;
      case Bond::Wedge : return Core::Bond::Wedge;
      case Bond::Hash : return Core::Bond::Hash;
      case Bond::WedgeOrHash : return Core::Bond::WedgeOrHash;
      case Bond::Thick : return Core::Bond::Thick;
      case Bond::Striped : return Core::Bond::Striped;
      case Bond::DoubleLegacy : return Core::Bond::DoubleLegacy;
      case Bond::CisOrTrans : return Core::Bond::CisOrTrans;
      case Bond::DoubleAsymmetric : return Core::Bond::DoubleAsymmetric;
      case Bond::DoubleSymmetric : return Core::Bond::DoubleSymmetric;
      case Bond::Triple : return Core::Bond::Triple;
      case Bond::TripleAsymmetric : return Core::Bond::TripleAsymmetric;
    }
    return Core::Bond::Invalid;
  }

  Core::Molecule Molecule::toCoreMolecule(qreal scaling) const {
    QVector<Core::Atom> catoms;
    QVector<Core::Bond> cbonds;
    for (auto atom : atoms())
      catoms << Core::Atom(atom->element(), atom->pos() / scaling, atom->numImplicitHydrogens(), atom->charge());
    for (auto bond : bonds())
      cbonds << Core::Bond(atoms().indexOf(bond->beginAtom()), atoms().indexOf(bond->endAtom()),
                           toCoreBondType(bond->bondType()));
    return Core::Molecule(catoms, cbonds, getName());
  }
} // namespace
