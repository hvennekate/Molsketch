/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   Copyright (C) 2018 (updated) by Hendrik Vennekate                     *
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

#include "molview.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>
#include "molscene.h"
#include "scenesettings.h"

#include <QMenu>
#include <QToolBar>
#include <math.h>

const qreal SCALING_FACTOR = 2.;

namespace Molsketch {

  struct MolViewPrivate {
  };

  MolView::MolView(MolScene *scene)
    : QGraphicsView(scene),
      d_ptr(new MolViewPrivate)
  {
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setMouseTracking(true);
    setAcceptDrops(true);
    setRenderHints(QPainter::Antialiasing);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setStyleSheet("");
    setContextMenuPolicy(Qt::DefaultContextMenu);

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  }

  MolView::~MolView() {}

  MolView *MolView::createView(SettingsFacade *sceneSettingsFacade) {
    MolScene *scene = new MolScene(new SceneSettings(sceneSettingsFacade));
    MolView *view = new MolView(scene);
    scene->setParent(view);
    return view;
  }

  void MolView::zoomIn() {
    scale(SCALING_FACTOR, SCALING_FACTOR);
  }

  void MolView::zoomOut() {
    scale(1./SCALING_FACTOR, 1./SCALING_FACTOR);
  }

  void MolView::zoomReset() {
    resetTransform();
  }

  void MolView::zoomFit() {
    if (!scene()) return;
    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
  }

  void MolView::scaleView(qreal scaleFactor) {
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRect(0,0,1,1)).width();
    if (factor < 0.07 || factor > 100) return;
    scale(scaleFactor, scaleFactor);
  }

  MolScene *MolView::scene() const {
    return dynamic_cast<MolScene*>(QGraphicsView::scene());
  }
}
