#ifndef MOLSKETCH_TEXTLINE_H
#define MOLSKETCH_TEXTLINE_H

#include <QPainter>
#include <QRectF>

namespace Molsketch {

  class TextBox;

  class TextLine
  {
    class TextLinePrivate;
    TextLinePrivate *d;
  public:
    explicit TextLine(const TextBox *centerBox);
    ~TextLine();
    QRectF boundingBox() const;
    void render(QPainter *painter) const;
    void addBoxRight(const TextBox *newBox);
    void addBoxLeft(const TextBox *newBox);
  };

} // namespace Molsketch

#endif // MOLSKETCH_TEXTLINE_H
