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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QtGlobal>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneMouseEvent>

#include <math.h>

#include "qtdeprecations.h"
#include "bond.h"

#include "atom.h"
#include "element.h"
#include "molscene.h"
#include "math2d.h"
#include "molecule.h"
#include <actions/bondtypeaction.h>
#include <actions/flipbondaction.h>
#include <actions/flipstereobondsaction.h>
#include "scenesettings.h"
#include "settingsitem.h"
#include <QDebug>

#define CHECKFORATOMS if (!m_beginAtom || !m_endAtom || !molecule())

namespace Molsketch {

  const QVector<QPair<qreal, qreal> > &HASH_SECTIONS {
    {0., .08},
    {.23, .31},
    {.46, .54},
    {.69, .77},
    {.92, 1.}
  };

  int Bond::orderFromType(const Bond::BondType &type) {
    return type / 10;
  }

  Bond::BondType Bond::simpleTypeFromOrder(const int &order)
  {
    switch (order)
    {
      case 1: return Single;
      case 2: return DoubleLegacy;
      case 3: return Triple;
      default: return Invalid;
    }
  }

  Bond::Bond(Atom* atomA, Atom* atomB, Bond::BondType type, QGraphicsItem* parent)
    : graphicsItem (parent),
      m_bondType(type),
      m_beginAtom(0),
      m_endAtom(0)
  {
    setAtoms(atomA, atomB);

    MolScene* molScene = dynamic_cast<MolScene*>(scene());
    if (molScene)
      setColor(molScene->settings()->defaultColor()->get());
    else
      setColor(QColor(0, 0, 0));

    setZValue(2);
  }

  Bond::Bond(const Bond &other, Atom* atomA, Atom* atomB)
    : graphicsItem(other),
      m_bondType(other.m_bondType),
      m_beginAtom(0),
      m_endAtom(0)
  {
    setAtoms(atomA, atomB);
  }

  Bond::~Bond() {}

  QRectF Bond::boundingRect() const
  {
    CHECKFORATOMS return QRect() ;
    qreal w = m_endAtom->x() - m_beginAtom->x();
    qreal h = m_endAtom->y() - m_beginAtom->y();
    return QRectF(mapFromParent(m_beginAtom->pos()) - QPointF(5,5), QSizeF(w+10,h+10)).normalized();
  }

  qreal Bond::bondAngle(const Atom *origin) const
  {
    CHECKFORATOMS return 0 ;
    return Molecule::toDegrees(bondAxis().angle() + (origin == endAtom()) * 180.) ;
  }

  QLineF Bond::bondAxis() const
  {
    CHECKFORATOMS return QLineF();
    return QLineF(mapFromParent(m_beginAtom->pos()),
                  mapFromParent(m_endAtom->pos())) ;
  }

  QLineF limitLineToExtents(const QLineF& line, qreal beginExtent, qreal endExtent) {
    return QLineF{line.pointAt(beginExtent), line.pointAt(endExtent)};
  }

  QPair<QLineF, QLineF> limitLinesToExtents(const QPair<QLineF, QLineF> &lines, qreal beginExtent, qreal endExtent) {
    return qMakePair(limitLineToExtents(lines.first, beginExtent, endExtent),
                     limitLineToExtents(lines.second, beginExtent, endExtent));
  }

  QPainterPath toClosedBondPath(const QPair<QLineF, QLineF> &outerLines) {
    QPainterPath path(outerLines.first.p1());
    path.lineTo(outerLines.first.p2());
    path.lineTo(outerLines.second.p2());
    path.lineTo(outerLines.second.p1());
    path.closeSubpath();
    return path;
  }

  QPainterPath Bond::drawBondComprisedOfStripes(const QPair<QLineF, QLineF> &outerLines) const {
    auto beginExtent = getExtentForStereoBond(beginAtom(), outerLines, false),
        endExtent = getExtentForStereoBond(endAtom(), outerLines, true);

    auto outerLinesToDraw = limitLinesToExtents(outerLines, beginExtent, endExtent);

    QPainterPath path;
    for (auto range : HASH_SECTIONS)
      path.addPath(toClosedBondPath(limitLinesToExtents(outerLinesToDraw, range.first, range.second)));

    return path;
  }

  QPainterPath Bond::drawHashBond() const {
    return drawBondComprisedOfStripes(getOuterLimitsOfStereoBond());
  }

  QPair<QLineF, QLineF> Bond::getOuterLimitsOfStereoBond() const {
    auto axis = bondAxis();
    auto normalVector = axis.normalVector().unitVector();
    if (MolScene* s = qobject_cast<MolScene*>(scene()))
      normalVector.setLength(s->settings()->bondWedgeWidth()->get()/2.);
    normalVector.translate(axis.dx(), axis.dy());

    return qMakePair(QLineF{axis.p1(), normalVector.p2()},
                     QLineF{axis.p1(), normalVector.pointAt(-1)});
  }

  QPair<QLineF, QLineF> Bond::getOuterLimitsOfThickBond() const {
    auto axis = bondAxis();
    auto normalVector = axis.normalVector().unitVector();
    if (MolScene* s = qobject_cast<MolScene*>(scene()))
      normalVector.setLength(s->settings()->bondWedgeWidth()->get()/2.);
    auto endNormalVector = normalVector.translated(axis.dx(), axis.dy());

    return qMakePair(QLineF{normalVector.p2(), endNormalVector.p2()},
                     QLineF{normalVector.pointAt(-1), endNormalVector.pointAt(-1)});
  }

  QLineF Bond::mapOuterLineToAtom(const Atom *atom, const QLineF& line, bool reverse) const {
   return QLineF(mapToItem(atom, reverse ? line.p2() : line.p1()),
                 mapToItem(atom, reverse ? line.p1() : line.p2()));
  }

  qreal Bond::getExtentForStereoBond(const Atom *atom, const QPair<QLineF, QLineF> &outerLines, bool reverse) const {
    if (reverse) return 1 - atom->getBondExtent(mapOuterLineToAtom(atom, outerLines.first, reverse),
                                                mapOuterLineToAtom(atom, outerLines.second, reverse),
                                                lineWidth());
    return atom->getBondExtent(mapOuterLineToAtom(atom, outerLines.first, reverse),
                               mapOuterLineToAtom(atom, outerLines.second, reverse),
                               lineWidth());
  }

  QPainterPath Bond::drawWedgeBond() const
  {
    auto outerLines = getOuterLimitsOfStereoBond();
    auto beginExtent = getExtentForStereoBond(beginAtom(), outerLines, false),
        endExtent = getExtentForStereoBond(endAtom(), outerLines, true);
    auto outerLinesToDraw = limitLinesToExtents(outerLines, beginExtent, endExtent);
    return toClosedBondPath(outerLinesToDraw);
  }

  QPainterPath Bond::drawWavyBond() const {
    auto outerLines = getOuterLimitsOfThickBond();
    auto beginExtent = getExtentForStereoBond(beginAtom(), outerLines, false),
        endExtent = getExtentForStereoBond(endAtom(), outerLines, true);

    MolScene* s = qobject_cast<MolScene*>(scene());
    auto circleDiameter = s ? s->settings()->bondWedgeWidth()->get() : 1; // TODO use dedicated setting
    auto axis = limitLineToExtents(bondAxis(), beginExtent, endExtent);
    int segments = qCeil(axis.length() / circleDiameter);
    QPainterPath path(axis.p1());
    for (int i = 0 ; i < segments ; ++i) {
      QRectF rect;
      rect.setWidth(circleDiameter);
      rect.setHeight(circleDiameter);
      rect.moveCenter(axis.pointAt((i + 0.5) * circleDiameter / axis.length()));
      path.arcTo(rect, axis.angle() + 180, i % 2 ? 180 : -180);
    }
    return path;
  }

  QPainterPath Bond::drawThickBond() const {
    auto outerLines = getOuterLimitsOfThickBond();
    auto beginExtent = getExtentForStereoBond(beginAtom(), outerLines, false),
        endExtent = getExtentForStereoBond(endAtom(), outerLines, true);
    auto outerLinesToDraw = limitLinesToExtents(outerLines, beginExtent, endExtent);
    return toClosedBondPath(outerLinesToDraw);
  }

  QPainterPath Bond::drawStripedBond() const {
    return drawBondComprisedOfStripes(getOuterLimitsOfThickBond());
  }

  QLineF shiftAndElongate(const QLineF &line, const QPointF &shift, const QPointF &elongation) {
    auto newLine = line.translated(shift);
    newLine.setPoints(newLine.p1() - elongation, newLine.p2() + elongation);
    return newLine;
  }

  QPainterPath Bond::getBondShapeFromOuterLines(const QPair<QLineF, QLineF> &outerLines) const {
    auto beginExtent = getExtentForStereoBond(beginAtom(), outerLines, false),
        endExtent = getExtentForStereoBond(endAtom(), outerLines, true);

    auto outerLinesToDraw = limitLinesToExtents(outerLines, beginExtent, endExtent);

    auto axis = bondAxis();
    axis.setLength(bondShapeGap());
    QPointF bondShift{axis.dx(), axis.dy()};
    QPointF normalShift{axis.dy(), -axis.dx()};

    auto shiftedOuterLines = qMakePair(shiftAndElongate(outerLinesToDraw.first, normalShift, bondShift),
              shiftAndElongate(outerLinesToDraw.second, -normalShift, bondShift));

    return toClosedBondPath(shiftedOuterLines);
  }

  QPainterPath Bond::getWedgeBondShape() const {
    return getBondShapeFromOuterLines(getOuterLimitsOfStereoBond());
  }

  QPainterPath Bond::getThickBondShape() const
  {
    return getBondShapeFromOuterLines(getOuterLimitsOfThickBond());
  }

  double minimumAngle(const Bond* reference, const QSet<Bond*>& others, const Atom* origin, bool clockwise)
  {
    double minAngle = 361;
    for (auto b : others)
    {
      double angle = b->bondAngle(origin) - reference->bondAngle(origin);
      if (clockwise) angle = 360 - angle;
      angle = Molecule::toDegrees(angle);
      minAngle = qMin(minAngle, angle);
    }
    return minAngle;
  }

  void Bond::determineDoubleBondOrientation()
  {
    if (m_bondType != DoubleLegacy) return;
    m_bondType = DoubleSymmetric;
    auto beginBondList = m_beginAtom->bonds();
    auto beginBonds = toSet(beginBondList);
    beginBonds -= this;
    auto endBondList = m_endAtom->bonds();
    auto endBonds = toSet(endBondList);
    endBonds -= this;
    // no other bonds: symmetric
    if (beginBonds.empty() && endBonds.empty()) return;
    // other bonds: add up angles upper and lower
    double sumUpperAngles = minimumAngle(this, beginBonds, m_beginAtom, false)
        + minimumAngle(this, endBonds, m_endAtom, true);
    double sumLowerAngles = minimumAngle(this, beginBonds, m_beginAtom, true)
        + minimumAngle(this, endBonds, m_endAtom, false);
    // equal (float tolerance): symmetric
    if (qAbs(sumUpperAngles - sumLowerAngles) < 1e-7) return;
    // else: unsymmetric
    m_bondType = DoubleAsymmetric;
    // consider swapping atoms
    if (sumUpperAngles > sumLowerAngles) qSwap(m_beginAtom, m_endAtom);
  }

  QLineF effectiveBondLine(const Bond* b, const Atom* a)
  {
    QLineF bl(b->bondAxis());
    if (b->beginAtom() != a)
      return QLineF(bl.p2(), bl.p1());
    return bl;
  }

  qreal findIdealAngle(const Atom* atom, const Bond* bond, bool inverted)
  {
    qreal angle = 120; // normal angle (e.g. in benzene)
    QLineF bondLine(effectiveBondLine(bond, atom));
    // TODO skip if we have a displayed label for the beginAtom
    foreach (const Bond* otherBond, atom->bonds())
    {
      if (otherBond == bond) continue;
      QLineF otherBondAxis(effectiveBondLine(otherBond, atom));
      angle = qMin(angle, (inverted
                           ? otherBondAxis.angleTo(bondLine)
                           : bondLine.angleTo(otherBondAxis)));
    }
    return angle*M_PI/360.; // includes division by 2
  }

  QPainterPath Bond::bondPath() const {
    // Get beginning and end
    QPointF begin = determineBondDrawingStart(m_beginAtom, m_endAtom);
    QPointF end = determineBondDrawingStart(m_endAtom, m_beginAtom);
    QPointF vb = end - begin;
    QPointF uvb = vb / sqrt(vb.x()*vb.x() + vb.y()*vb.y());
    if (MolScene* s = qobject_cast<MolScene*>(scene()))
      uvb *= s->settings()->bondSeparation()->get();
    QPointF normalVector(uvb.y(), -uvb.x());

    QPainterPath result;
    switch ( m_bondType ) // TODO beautify
    {
      case Bond::Single:
      case Bond::DativeDot:
      case Bond::DativeDash:
        result.moveTo(begin);
        result.lineTo(end);
        break;
      case Bond::WedgeOrHash:
        return drawWavyBond();
      case Bond::Thick:
        return drawThickBond();
      case Bond::Striped:
        return drawStripedBond();
      case Bond::Wedge:
        return drawWedgeBond();
      case Bond::Hash:
        return drawHashBond();
      case Bond::DoubleSymmetric:
        {
          QPointF offset = .5*normalVector;
          result.moveTo(begin + offset);
          result.lineTo(end + offset);
          result.moveTo(begin - offset);
          result.lineTo(end - offset);
          break;
        }
      case Bond::CisOrTrans:
        {
          QPointF offset = .5*normalVector;
          result.moveTo(begin + offset);
          result.lineTo(end - offset);
          result.moveTo(begin - offset);
          result.lineTo(end + offset);
          break;
        }
      case Bond::DoubleAsymmetric:
        {
          result.moveTo(begin);
          result.lineTo(end);
          // now the double part
          QPointF offset = normalVector;
          qreal beginAngle = findIdealAngle(beginAtom(), this, false),
              endAngle = findIdealAngle(endAtom(), this, true),
              limitAngle = atan(2*QLineF(QPointF(0,0),uvb).length()/QLineF(begin, end).length());
          beginAngle = qMax(beginAngle, limitAngle);
          endAngle = qMax(endAngle, limitAngle);
          result.moveTo(begin + uvb/tan(beginAngle) + offset);
          result.lineTo(end - uvb/tan(endAngle) + offset);
          break;
        }
      case Bond::Triple:
        {
          result.moveTo(begin);
          result.lineTo(end);
          QPointF offset = normalVector;
          result.moveTo(begin + offset);
          result.lineTo(end + offset);
          result.moveTo(begin - offset);
          result.lineTo(end - offset);
          break;
        }
      case TripleAsymmetric:
        {
          result.moveTo(begin);
          result.lineTo(end);
          // now the double part
          QPointF offset = normalVector;
          qreal beginAngle = findIdealAngle(beginAtom(), this, false),
              endAngle = findIdealAngle(endAtom(), this, true),
              limitAngle = atan(2*QLineF(QPointF(0,0),uvb).length()/QLineF(begin, end).length());
          beginAngle = qMax(beginAngle, limitAngle);
          endAngle = qMax(endAngle, limitAngle);
          result.moveTo(begin + uvb/tan(beginAngle) + offset);
          result.lineTo(end - uvb/tan(endAngle) + offset);
          beginAngle = findIdealAngle(beginAtom(), this, true);
          endAngle = findIdealAngle(endAtom(), this, false);
          beginAngle = qMax(beginAngle, limitAngle);
          endAngle = qMax(endAngle, limitAngle);
          result.moveTo(begin + uvb/tan(beginAngle) - offset);
          result.lineTo(end - uvb/tan(endAngle) - offset);
          break;
        }
      default: ;
    }
    return result;
  }

  QPointF Bond::determineBondDrawingStart(Atom *start, Atom *end) const {
    return mapFromScene(start->bondDrawingStart(end, lineWidth()));
  }

  QList<Bond *> Bond::coveringBonds() const
  {
    QList<Bond*> result;
    auto sc = scene();
    if (!sc) return result;
    for (auto item : sc->items())
      if (auto bond = dynamic_cast<Bond*>(item))
        if (bond->zValue() > zValue() && collidesWithItem(bond))
          result << bond;
    return result;
  }

  QPainterPath brokenBondIndicator(const QPointF &point, const QPointF &bondVector, const QPointF &normalVector) {
    QPointF bondUnitVector(bondVector/QLineF(QPointF(0,0), bondVector).length());
    const qreal decorationScale = 0.2; // TODO modifiable - from wedge bond width!
    QPointF
        x = normalVector*decorationScale,
        y = -8*bondUnitVector*decorationScale;
    QPainterPath path;
    path.moveTo(-7*x);
    path.quadTo(-7*x+y, -6*x+y);
    path.cubicTo(-5*x+y, -5*x, -4*x);
    path.cubicTo(-3*x, -3*x+y, -2*x+y);
    path.cubicTo(-x+y, -x, 0*x);
    path.cubicTo(x, x+y, 2*x+y);
    path.cubicTo(3*x+y, 3*x, 4*x);
    path.cubicTo(5*x, 5*x+y, 6*x+y);
    path.quadTo(7*x+y, 7*x);
    path.translate(point + 0.3 * bondVector);

    return path;
  }

  QPainterPath Bond::getBrokenBondIndicatorsPath(const QPointF &begin, const QPointF &end, const QPointF &normalVector) const {
    QPointF bondUnitVector((end-begin)/QLineF(end, begin).length());
    const qreal decorationScale = 0.2; // TODO modifiable - from wedge bond width!
    QPointF
        x = normalVector*decorationScale,
        y = 8*bondUnitVector*decorationScale,
        xl = normalVector/QLineF(QPointF(), normalVector).length() * bondShapeGap() / 1.5,
        yl = bondUnitVector * bondShapeGap() / 1.5;
    QPainterPath path;
    path.moveTo(-7*x - xl - y - yl);
    path.lineTo(+7*x + xl - y - yl);
    path.lineTo(+7*x + xl + yl);
    path.lineTo(-7*x - xl + yl);
    path.closeSubpath();
    return path.translated(begin + 0.3 * (end - begin));
  }


  QPainterPath Bond::clipBrokenBondIndicator(const QPointF& point,
                     const QPointF& otherAtom,
                     const QPointF& normalVector) const
  {
    QPointF bondVector = otherAtom - point;
    QPointF bondVectorOffset = bondVector / QLineF(QPointF(), bondVector).length() * 2 * lineWidth();
    QPainterPath path(brokenBondIndicator(point, bondVector, normalVector));
    path.lineTo(point + 1.4 * normalVector - bondVectorOffset); // TODO get from wedge bond width + linewidth
    path.lineTo(point - 1.4 * normalVector - bondVectorOffset); // TODO get from wedge bond width + linewidth
    path.closeSubpath();
    return path;
  }

  void drawBrokenIndicator(QPainter* painter,
                           const QPainterPath& path)
  {
    QPen subPen(painter->pen());
    subPen.setWidthF(subPen.widthF()* 0.75); // TODO fix shape box accordingly
    painter->save();
    painter->setPen(subPen);
    painter->drawPath(path);
    painter->restore();
  }

  void Bond::paintBrokenBondIndicators(QPainter *painter, const QPointF &begin, const QPointF &end, const QPointF &vb, const QPointF &normalVector) {
    if (m_beginAtom->element().isEmpty()) drawBrokenIndicator(painter, brokenBondIndicator(begin, vb, normalVector));
    if (m_endAtom->element().isEmpty()) drawBrokenIndicator(painter, brokenBondIndicator(end, -vb, normalVector));
  }

  void Bond::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
  {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    CHECKFORATOMS return ;

    if (m_bondType == DoubleLegacy) determineDoubleBondOrientation();

    QRectF startRect = m_beginAtom->mapRectToItem(this, m_beginAtom->boundingRect()),
        endRect = m_endAtom->mapRectToItem(this, m_endAtom->boundingRect());
    if (startRect.intersects(endRect) && m_beginAtom->isDrawn() && m_endAtom->isDrawn()) return; // TODO should not be necessary anymore


    // Get beginning and end
    QPointF begin = mapFromParent(m_beginAtom->pos());
    QPointF end = mapFromParent(m_endAtom->pos());
    QPointF vb = end - begin;
    QPointF uvb = vb / sqrt(vb.x()*vb.x() + vb.y()*vb.y());
    if (MolScene* s = qobject_cast<MolScene*>(scene()))
      uvb *= s->settings()->bondSeparation()->get();
    QPointF normalVector(uvb.y(), -uvb.x());

    begin = determineBondDrawingStart(m_beginAtom, m_endAtom);
    end = determineBondDrawingStart(m_endAtom, m_beginAtom);
    // TODO this collision detection rests on the factor used in Atom for the determination of the starting point. Improve!
    if (m_beginAtom->contains(mapToItem(m_beginAtom, end
                                        + 0.75 * lineWidth() * QLineF(QPointF(), m_beginAtom->pos() - m_endAtom->pos()).unitVector().p2()))
        || m_endAtom->contains(mapToItem(m_endAtom, begin
                                         + 0.75* lineWidth() * QLineF(QPointF(), m_endAtom->pos() - m_beginAtom->pos()).unitVector().p2())))
      return;

    QPen pen;

    painter->save();
    pen.setWidthF(lineWidth());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(getColor());
    painter->setPen(pen);

    QPainterPath coveringShapes;
    for (auto coveringBond : coveringBonds())
      coveringShapes += mapFromItem(coveringBond, coveringBond->shape());

    begin = mapFromParent(m_beginAtom->pos());
    end = mapFromParent(m_endAtom->pos());
    painter->setClipPath(shape().subtracted(coveringShapes));
    paintBrokenBondIndicators(painter, begin, end, vb, normalVector);
    if (m_beginAtom->element().isEmpty())
      painter->setClipPath(painter->clipPath().subtracted(clipBrokenBondIndicator(begin, end, normalVector)));
    if (m_endAtom->element().isEmpty())
      painter->setClipPath(painter->clipPath().subtracted(clipBrokenBondIndicator(end, begin, normalVector)));

    switch ( m_bondType ) // TODO beautify
    {
      case Bond::DativeDot:
        pen.setStyle(Qt::DotLine);
        break;
      case Bond::DativeDash:
        pen.setStyle(Qt::DashLine);
        break;
      case Bond::Wedge:
      case Bond::Hash:
      case Bond::Thick:
      case Bond::Striped:
          painter->setBrush( QBrush(getColor()) );
      default: ;
    }
    painter->setPen(pen);
    QPainterPath path = bondPath();
    painter->drawPath(path);

    painter->setClipping(false);
    painter->setBrush(Qt::NoBrush);

    if (isSelected()) {
      painter->setPen(Qt::blue);
      painter->drawPath(shape());
    }
    painter->restore();

    graphicsItem::paint(painter, option, widget);
  }

  QVariant Bond::itemChange(GraphicsItemChange change, const QVariant &value)
  {
    if (change == ItemPositionChange && parentItem()) parentItem()->update();
    return graphicsItem::itemChange(change, value);
  }

  QPainterPath Bond::shape() const
  {
    CHECKFORATOMS return QPainterPath() ;
    return outline();
  }

  void Bond::setType(const BondType &t)
  {
    m_bondType = t;
    if (Molecule *m = molecule()) {
      m->updateElectronSystems();
      m->updateTooltip();
    }
    update();
  }

  int Bond::bondOrder() const
  {
    return orderFromType(bondType());
  }

  Bond::BondType Bond::bondType() const
  {
    return m_bondType;
  }

  Atom* Bond::beginAtom() const
  {
    return m_beginAtom;
  }

  Atom* Bond::endAtom() const
  {
    return m_endAtom;
  }

  bool Bond::hasAtom(const Atom* atom) const
  {
    return m_beginAtom == atom || m_endAtom == atom;
  }

  Atom* Bond::otherAtom(const Atom *atom) const
  {
    return (atom == m_beginAtom) ? m_endAtom : m_beginAtom;
  }

  void Bond::setAtoms(Atom *A, Atom *B)
  {
    m_beginAtom = A ;
    m_endAtom = B ;
    if (m_beginAtom) {
      m_beginAtom->updateLabel();
      setPos(m_beginAtom->scenePos()) ;
    }
    if (m_endAtom) m_endAtom->updateLabel();
  }

  void Bond::setAtoms(const QPair<Atom *, Atom *> &atoms) {
    setAtoms(atoms.first, atoms.second);
  }

  QPair<Atom *, Atom *> Bond::atoms() const {
    return qMakePair(beginAtom(), endAtom());
  }

  Molecule* Bond::molecule() const
  {
    return dynamic_cast<Molecule*>(this->parentItem());
  }

  QLineF Bond::shiftVector(const QLineF &vector, qreal shift) // Shifts a vector on the perpendicular axis
  {
    qreal rx1 = vector.x1() + shift*(vector.unitVector().y2()-vector.unitVector().y1());
    qreal ry1 = vector.y1() + shift*-(vector.unitVector().x2()-vector.unitVector().x1());
    qreal rx2 = vector.x2() + shift*(vector.unitVector().y2()-vector.unitVector().y1());
    qreal ry2 = vector.y2() + shift*-(vector.unitVector().x2()-vector.unitVector().x1());
    return QLineF(rx1,ry1,rx2,ry2);
  }

  QString Bond::xmlName() const { return xmlClassName(); }

  QString Bond::xmlClassName() { return "bond" ; }

  void Bond::setCoordinates(const QVector<QPointF> &c)
  {
    if (c.size() != 2) return ;
    CHECKFORATOMS return;
    m_beginAtom->setCoordinates(c.mid(0,1)) ;
    m_endAtom->setCoordinates(c.mid(1,1));
  }

  QPolygonF Bond::coordinates() const
  {
   CHECKFORATOMS return QVector<QPointF>();
    return QVector<QPointF>()
        << m_beginAtom->coordinates()
        << m_endAtom->coordinates() ;
  }

  QXmlStreamAttributes Bond::graphicAttributes() const
  {
    QXmlStreamAttributes attributes ;
    attributes.append("atomRefs2", m_beginAtom->index() + " " + m_endAtom->index()) ;
    attributes.append("type", QString::number(m_bondType));
    return attributes ;
  }

  void Bond::readGraphicAttributes(const QXmlStreamAttributes &attributes)
  {
    QStringList atomIndexes = attributes.value("atomRefs2").toString().split(" ") ;
    if (atomIndexes.size() != 2) return ;

    if (auto mol = molecule())
      setAtoms(mol->atom(atomIndexes.first()),
               mol->atom(atomIndexes.last())) ;
    m_bondType = (BondType) (attributes.value("type").toString().toInt());
    if (attributes.hasAttribute("order"))
      m_bondType = (BondType) (10 *attributes.value("order").toInt());
  }

  void Bond::prepareContextMenu(QMenu *contextMenu) // TODO simply use a function that returns the scene's actions pertaining to the item
  {
    // Prepare bond menu
    MolScene *sc = qobject_cast<MolScene*>(scene());
    if (sc)
    {
      QList<QAction*> actions;
      actions << sc->findChild<bondTypeAction*>()
              << sc->findChild<flipBondAction*>()
              << sc->findChild<flipStereoBondsAction*>();
      foreach(QAction* action, actions)
      {
        if (!action) continue;
        QObject::connect(action, SIGNAL(triggered()), contextMenu, SLOT(close())); // TODO check if "changed()" event can accomplish this. Only required for bondType action
        contextMenu->addAction(action);
      }
    }
    graphicsItem::prepareContextMenu(contextMenu);
  }

  class LegacyBondStereo : public XmlObjectInterface {
    Bond *bond;
  public:
    LegacyBondStereo(Bond *bond) : bond(bond) {}
    QXmlStreamReader& readXml(QXmlStreamReader &in) override {
      auto text = in.readElementText();
      if ("H" == text) bond->setType(Bond::Hash);
      if ("W" == text) bond->setType(Bond::Wedge);
      return in;
    }

    QXmlStreamWriter& writeXml(QXmlStreamWriter &out) const override {
      return out;
    }
  };

  XmlObjectInterface *Bond::produceChild(const QString &name, const QXmlStreamAttributes &attributes) {
    if (name != "bondStereo" || !attributes.isEmpty()) return nullptr;
    XmlObjectInterface *helper = new LegacyBondStereo(this);
    helpers << helper;
    return helper;
  }

  void Bond::afterReadFinalization()
  {
    for (auto helper : helpers) delete helper;
    helpers.clear();
  }

  qreal Bond::bondShapeGap() const {
    return lineWidth(); // TODO make this configurable!
  }

  QPainterPath Bond::outline() const {
    // Get beginning and end (taken from bondPath()
    QPointF begin = determineBondDrawingStart(m_beginAtom, m_endAtom);
    QPointF end = determineBondDrawingStart(m_endAtom, m_beginAtom);
    QPointF vb = end - begin;
    QPointF uvb = vb / sqrt(vb.x()*vb.x() + vb.y()*vb.y());
    QPointF bondUnitVector(uvb);
    QPointF bondNormalVector(uvb.y(), -uvb.x());
    if (MolScene* s = qobject_cast<MolScene*>(scene()))
      uvb *= s->settings()->bondSeparation()->get();
    QPointF normalVector(uvb.y(), -uvb.x());

    QPainterPath result;

    qreal gap = bondShapeGap();

    switch (m_bondType) {
      case Bond::Single:
      case Bond::DativeDot:
      case Bond::DativeDash:
        result.moveTo(begin + gap * (-bondUnitVector + bondNormalVector));
        result.lineTo(begin + gap * (-bondUnitVector - bondNormalVector));
        result.lineTo(end + gap * (bondUnitVector - bondNormalVector));
        result.lineTo(end + gap * (bondUnitVector + bondNormalVector));
        result.closeSubpath();
        break;
      case Bond::WedgeOrHash:
      case Bond::Thick:
      case Bond::Striped:
        result = getThickBondShape();
        break;
      case Bond::Wedge:
      case Bond::Hash:
        result = getWedgeBondShape();
        break;
      case DoubleSymmetric:
        {
          QPointF offset = .5*normalVector;
          result.moveTo(begin + offset + gap * (-bondUnitVector + bondNormalVector));
          result.lineTo(end + offset + gap * (bondUnitVector + bondNormalVector));
          result.lineTo(end - offset + gap * (bondUnitVector - bondNormalVector));
          result.lineTo(begin - offset + gap * (-bondUnitVector - bondNormalVector));
          result.closeSubpath();
          break;
        }
      case DoubleAsymmetric:
        { // TODO add more gap
          result.moveTo(begin + gap * (-bondUnitVector - bondNormalVector));
          result.lineTo(end + gap * (bondUnitVector - bondNormalVector));
          // now the double part
          QPointF offset = normalVector;
          qreal beginAngle = findIdealAngle(beginAtom(), this, false),
              endAngle = findIdealAngle(endAtom(), this, true),
              limitAngle = atan(2*QLineF(QPointF(0,0),uvb).length()/QLineF(begin, end).length());
          beginAngle = qMax(beginAngle, limitAngle);
          endAngle = qMax(endAngle, limitAngle);
          result.lineTo(end - uvb/tan(endAngle) + offset + gap * (bondUnitVector + bondNormalVector));
          result.lineTo(begin + uvb/tan(beginAngle) + offset + gap * (-bondUnitVector + bondNormalVector));
          result.closeSubpath();
          break;
        }
      case CisOrTrans:
        {
          QPointF offset = .5*normalVector;
          result.moveTo(begin + offset + gap * (-bondUnitVector + bondNormalVector));
          result.lineTo((begin + end)/2 + gap * bondNormalVector);
          result.lineTo(end + offset + gap * (bondUnitVector + bondNormalVector));
          result.lineTo(end - offset + gap * (bondUnitVector - bondNormalVector));
          result.lineTo((begin + end)/2 + gap * -bondNormalVector);
          result.lineTo(begin - offset + gap * (-bondUnitVector - bondNormalVector));
          result.closeSubpath();
          break;
        }
      case Bond::Triple:
        {
          QPointF offset = normalVector;
          result.moveTo(begin + offset + gap * (-bondUnitVector + bondNormalVector));
          result.lineTo(end + offset + gap * (bondUnitVector + bondNormalVector));
          result.lineTo(end - offset + gap * (bondUnitVector - bondNormalVector));
          result.lineTo(begin - offset + gap * (-bondUnitVector - bondNormalVector));
          result.closeSubpath();
          break;
        }
      case Bond::TripleAsymmetric:
        { // TODO add more gap to central line
          result.moveTo(begin + gap * -bondUnitVector);
          // now the double part
          QPointF offset = normalVector;
          qreal beginAngle = findIdealAngle(beginAtom(), this, false),
              endAngle = findIdealAngle(endAtom(), this, true),
              limitAngle = atan(2*QLineF(QPointF(0,0),uvb).length()/QLineF(begin, end).length());
          beginAngle = qMax(beginAngle, limitAngle);
          endAngle = qMax(endAngle, limitAngle);
          result.lineTo(begin + uvb/tan(beginAngle) + offset + gap * (-bondUnitVector + bondNormalVector));
          result.lineTo(end - uvb/tan(endAngle) + offset + gap * (bondUnitVector + bondNormalVector));
          result.lineTo(end + gap * bondUnitVector);
          beginAngle = findIdealAngle(beginAtom(), this, true);
          endAngle = findIdealAngle(endAtom(), this, false);
          beginAngle = qMax(beginAngle, limitAngle);
          endAngle = qMax(endAngle, limitAngle);
          result.lineTo(end - uvb/tan(endAngle) - offset + gap * (bondUnitVector - bondNormalVector));
          result.lineTo(begin + uvb/tan(beginAngle) - offset + gap * (-bondUnitVector - bondNormalVector));
          result.closeSubpath();
          break;
        }
      default: ;
    }
    if (m_beginAtom->element().isEmpty())
      (result -= clipBrokenBondIndicator(mapFromParent(m_beginAtom->pos()), mapFromParent(m_endAtom->pos()), normalVector))
        += getBrokenBondIndicatorsPath(mapFromParent(m_beginAtom->pos()), mapFromParent(m_endAtom->pos()), normalVector);
    if (m_endAtom->element().isEmpty())
      (result -= clipBrokenBondIndicator(mapFromParent(m_endAtom->pos()), mapFromParent(m_beginAtom->pos()), normalVector))
        += getBrokenBondIndicatorsPath(mapFromParent(m_endAtom->pos()), mapFromParent(m_beginAtom->pos()), normalVector);
    return result;
  }
} // namespace
