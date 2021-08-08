#ifndef TESTTEXTBOX_H
#define TESTTEXTBOX_H

#include "mocks.h"
#include "painting/textbox.h"
#include "utilities.h"

using namespace Molsketch;

class TestTextBox : public TextBox, public ForTesting<TextBox> {
public:
  TestTextBox() : TextBox(QFont()) {}
  MOCK_CONST(QRectF, boundingRect, , )
  VOID_MOCK_CONST(paint, QPainter* painter, painter)
  static std::function<void(const int&)> destructorCallback;
  void storePainterTransform(QTransform &transform);
};

#endif // TESTTEXTBOX_H
