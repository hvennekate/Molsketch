/***************************************************************************
 *   Copyright (C) 2021 Hendrik Vennekate                                  *
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
#ifndef DRAWINGTESTDIALOG_H
#define DRAWINGTESTDIALOG_H

#include <QDialog>

class QGraphicsScene;
class QGraphicsPixmapItem;

namespace Ui {
  class DrawingTestDialog;
}

class DrawingTestDialog : public QDialog
{
  Q_OBJECT
private:
  QPicture *drawInternal() const;
public:
  class Drawable {
  public:
    virtual void draw(QPainter* painter) = 0;
    virtual QRectF boundingRect() const = 0;
    virtual ~Drawable(){}
  };
  explicit DrawingTestDialog(Drawable *drawable, QWidget *parent = nullptr);
  explicit DrawingTestDialog(QVector<Drawable*> drawables, QWidget *parent = nullptr);
  ~DrawingTestDialog();
public slots:
  void redraw() const;

private:
  QScopedPointer<Ui::DrawingTestDialog> ui;
  QVector<QSharedPointer<Drawable>> drawables;
  QScopedPointer<QGraphicsScene> scene;
  QGraphicsPixmapItem *graphicsItem;
};

#endif // DRAWINGTESTDIALOG_H
