#include <QPainter>
#include "testtextbox.h"

void TestTextBox::storePainterTransform(QTransform &transform) {
  paintCallback = [&] (QPainter *painter) {
    transform = painter->transform();
  };
}

bool TestTextBox::preferredCenter() const {
  return true;
}
