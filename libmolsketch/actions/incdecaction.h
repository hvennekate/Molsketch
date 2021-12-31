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
#ifndef INCDECACTION_H
#define INCDECACTION_H

#include "multiaction.h"

namespace Molsketch {
  class Atom ;
  class Bond;

  template <class T, typename I = int>
  class incDecAction : public multiAction {
  public:
    explicit incDecAction(MolScene* scene);
    ~incDecAction() ;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QAction* decrementAction() const;
    QAction* incrementAction() const;
  protected:
    void initialize(QIcon UpIcon,
                    QIcon DownIcon,
                    QString UpText,
                    QString DownText,
                    I (T::*getFunction)()const,
                    void (T::*setFunction)(const I&)) ;
  private:
    class privateData;
    privateData *d ;
    T* getItem(const QPointF& p) ;
  };

  class chargeAction : public incDecAction<Atom> {
    Q_OBJECT
  public:
    explicit chargeAction(MolScene *scene) ;
  } ;

  class hydrogenAction : public incDecAction<Atom, quint8> {
    Q_OBJECT
  public:
    explicit hydrogenAction(MolScene *scene) ;
  } ;

  class ZLevelStepAction : public incDecAction<Bond> {
    Q_OBJECT
  public:
    explicit ZLevelStepAction(MolScene *scene);
  };
} //namespace

#endif // INCDECACTION_H
