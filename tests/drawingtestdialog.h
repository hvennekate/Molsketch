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
