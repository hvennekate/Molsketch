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
#include "drawingtestdialog.h"
#include "ui_drawingtestdialog.h"

#include <QGraphicsPixmapItem>
#include <QPicture>

DrawingTestDialog::DrawingTestDialog(Drawable *drawable, QWidget *parent)
  : DrawingTestDialog(QVector<Drawable*>{drawable}, parent) {}

DrawingTestDialog::DrawingTestDialog(QVector<DrawingTestDialog::Drawable *> drawables, QWidget *parent)
  : QDialog(parent),
    ui(new Ui::DrawingTestDialog),
    scene(new QGraphicsScene),
    graphicsItem(new QGraphicsPixmapItem)
{
  ui->setupUi(this);
  ui->view->setScene(scene.get());
  scene->addItem(graphicsItem);

  std::transform(drawables.cbegin(), drawables.cend(), std::back_inserter(this->drawables), [](Drawable *d) { return QSharedPointer<Drawable>(d);});

  redraw();
}

DrawingTestDialog::~DrawingTestDialog() {}

void drawGrid(QPainter *painter, int size, int step) {
  painter->save();
  painter->setPen(QPen(Qt::gray, 1, Qt::DotLine));
  for (int i = -size; i <= size; i += step) {
    painter->drawLine(i, -size, i, size);
    painter->drawLine(-size, i, size, i);
  }
  painter->restore();
}

void drawOrigin(QPainter *painter, int size) {
  painter->save();
  painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
  painter->drawLine(0, -size, 0, size);
  painter->drawLine(-size, 0, size, 0);
  painter->restore();
}

void drawBounds(QPainter * painter, const QRectF &bounds) {
  painter->save();
  painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
  painter->drawRect(bounds);
  painter->restore();
}

QPicture *DrawingTestDialog::drawInternal() const {
  auto picture = new QPicture();
  auto painter = new QPainter(picture);
  auto size = ui->size->value();
  auto zoom = ui->zoom->value();
  auto canvasSize = size * 2 * zoom;
  painter->save();
  painter->fillRect(0, 0, canvasSize, canvasSize, Qt::white);
  painter->restore();
  painter->setTransform(QTransform().scale(zoom, zoom).translate(size, size));

  if (ui->drawGrid->isChecked()) drawGrid(painter, size, 10);
  if (ui->drawOrigin->isChecked()) drawOrigin(painter, size);
  if (ui->drawBounds->isChecked())
    for (auto drawable : drawables)
      drawBounds(painter, drawable->boundingRect());

  for (auto drawable : drawables) drawable->draw(painter);

  painter->end();
  delete painter;

  return picture;
}

void DrawingTestDialog::redraw() const {
  auto picture = drawInternal();
  auto size = ui->size->value();
  auto zoom = ui->zoom->value();
  auto canvasSize = size * 2 * zoom; // TODO function
  QPixmap pixmap(canvasSize, canvasSize);
  auto painter = new QPainter(&pixmap);
  picture->play(painter);
  painter->end();
  delete painter;

  graphicsItem->setPixmap(pixmap);
}


