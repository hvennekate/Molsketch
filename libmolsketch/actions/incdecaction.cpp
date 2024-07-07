/***************************************************************************
 *   Copyright (C) 2015 Hendrik Vennekate                                  *
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
#include "incdecaction.h"

#include <QGraphicsSceneMouseEvent>
#include <QUndoCommand>
#include "atom.h"
#include "bond.h"
#include "molscene.h"
#include "iconutils.h"

namespace Molsketch {

  template<class T, typename I>
  class incDecCommand : public QUndoCommand
  {
  private:
    T* t ;
    bool plus ;
    I (T::*getFunction)()const ;
    void (T::*setFunction)(const I&) ;
  public:
    incDecCommand(T* a,
                  bool increment,
                  I (T::*gf)()const,
                  void (T::*sf)(const I&),
                  QString text,
                  QUndoCommand* parent = 0)
      : QUndoCommand(text, parent),
        t(a),
        plus(increment),
        getFunction(gf),
        setFunction(sf)
    {
    }

    void redo() override {
      (t->*setFunction)((t->*getFunction)() + (plus ? 1 : -1)) ;
    }

    void undo() override {
      (t->*setFunction)((t->*getFunction)() + (plus ? -1 : 1)) ;
    }
    // TODO mergeable
  };

  template <class T, typename I>
  struct incDecAction<T, I>::privateData
  {
    privateData() :
      plusAction(0),
      minusAction(0)
    {}
    void clear()
    {
      delete plusAction ;
      plusAction = 0 ;
      delete minusAction ;
      minusAction = 0 ;
    }
    QAction *plusAction ;
    QAction *minusAction ;
    I (T::*getFunction)() const;
    void (T::*setFunction)(const I&) ;
  };

  template <class T, typename I>
  incDecAction<T, I>::incDecAction(MolScene *scene)
    : multiAction(scene),
      d(new privateData)
  {}

  template <class T, typename I>
  incDecAction<T, I>::~incDecAction()
  {
    delete d ;
  }

  template <class T, typename I>
  void incDecAction<T, I>::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    if (event->button() != Qt::LeftButton || event->modifiers()) return;
    event->accept();
    // Check possible targets
    T* t = getItem(event->buttonDownScenePos(event->button()));
    if (!t) return;

    undoStack()->push(new incDecCommand<T, I>(t,
                                        activeSubAction() == d->plusAction,
                                        d->getFunction,
                                        d->setFunction,
                                        activeSubAction()->text())) ;
  }

  template <class T, typename I>
  void incDecAction<T, I>::initialize(QIcon UpIcon,
                                   QIcon DownIcon,
                                   QString UpText,
                                   QString DownText,
                                   I (T::*getFunction)()const,
                                   void (T::*setFunction)(const I&))
  {
    d->clear();
    d->plusAction = new QAction(UpIcon, UpText, this) ;
    d->minusAction = new QAction(DownIcon, DownText, this) ;
    addSubAction(d->plusAction);
    addSubAction(d->minusAction);

    d->setFunction = setFunction ;
    d->getFunction = getFunction ;
  }

  template<class T>
  T *getItemAt(MolScene *scene, const QPointF &p) {
    Q_UNUSED(scene)
    Q_UNUSED(p)
    return nullptr;
  }

  template<>
  Atom *getItemAt(MolScene *scene, const QPointF &p) {
    return scene->atomNear(p) ;
  }

  template<>
  Bond *getItemAt(MolScene *scene, const QPointF &p) {
    return scene->bondAt(p);
  }

  template <class T, typename I>
  T *incDecAction<T, I>::getItem(const QPointF &p){
    return getItemAt<T>(scene(), p);
  }

  chargeAction::chargeAction(MolScene *scene)
    : incDecAction(scene)
  {
    setText(tr("Charge"));
    initialize(getInternalIcon("incCharge"),
               getInternalIcon("decCharge"),
               tr("Increase charge"),
               tr("Decrease charge"),
               &Atom::charge,
               &Atom::setCharge);
  }

  hydrogenAction::hydrogenAction(MolScene *scene)
    : incDecAction(scene)
  {
    setText(tr("Hydrogens"));
    initialize(getInternalIcon("incHydrogens"),
               getInternalIcon("decHydrogens"),
               tr("Add implicit hydrogen"),
               tr("Remove implicit hydrogen"),
               &Atom::numImplicitHydrogens,
               &Atom::setNumImplicitHydrogens);
  }

  template <class T, typename I>
  QAction *incDecAction<T, I>::decrementAction() const
  {
    return d->minusAction;
  }

  template <class T, typename I>
  QAction *incDecAction<T, I>::incrementAction() const
  {
    return d->plusAction;
  }

  ZLevelStepAction::ZLevelStepAction(MolScene *scene)
    : incDecAction(scene)
  {
    setText(tr("Drawing Level"));
    initialize(getInternalIcon("layerup"),
               getInternalIcon("layerdown"),
               tr("Move up"),
               tr("Move down"),
               &Bond::roundedZValue,
               &Bond::setZValue);
  }

  // instantiation
  template class incDecAction<Atom>;
  template class incDecAction<Atom, quint8>;
  template class incDecAction<Bond>;

} // namespace
