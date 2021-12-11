#include <QPainter>
#include "testtextbox.h"

QDebug TestTextBox::debug(QDebug debug) const {
  return debug << "TestTextBox";
}

void TestTextBox::storePainterTransform(QTransform &transform) {
  paintCallback = [&] (QPainter *painter) {
    transform = painter->transform();
  };
}

bool TestTextBox::preferredCenter() const {
  return true;
}
