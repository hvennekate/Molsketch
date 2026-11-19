/***************************************************************************
 *   Copyright (C) 2007-2008 by Harm van Eersel                            *
 *   devsciurus@xs4all.nl                                                  *
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

#ifndef molview_H
#define molview_H

#include <QGraphicsView>

#define MOUSEWHEELDIVIDER 120 // TODO change

class QToolBar;
class QMenu;

namespace Molsketch {
  class SettingsFacade;
  class MolScene;
  class MolViewPrivate;

  class MolView : public QGraphicsView
  {
    Q_OBJECT
    Q_DECLARE_PRIVATE(MolView)
    QScopedPointer<MolViewPrivate> d_ptr;
  public:
    explicit MolView(Molsketch::MolScene *scene);
    ~MolView();
    static MolView* createView(SettingsFacade*);
    MolScene *scene() const;
  public slots:
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void zoomFit();
    void scaleView(qreal scaleFactor);
  protected:
    void wheelEvent(QWheelEvent *event) override;
    bool viewportEvent(QEvent *event) override;
    bool event(QEvent *event) override;
  private:
    bool handlePinchGesture(QEvent *event);
  };

} // namespace
#endif
