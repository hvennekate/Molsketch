/***************************************************************************
 *   Copyright (C) 2017 by Hendrik Vennekate                               *
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

#include "commands.h"
#include "molscene.h"
#include "textitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QFocusEvent>
#include <QPainter>

namespace Molsketch {

  struct TextItem::privateData {
    bool initialFill;
    bool hovering;
  };

  TextItem::TextItem()
    : d(new privateData)
  {
    d->initialFill = true;
    d->hovering = false;
    setFlags(flags()
             | ItemAcceptsInputMethod
             | ItemIsFocusable
             | ItemIsMovable
             | ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setZValue(50);
  }

  QXmlStreamReader &TextItem::readXml(QXmlStreamReader &in)
  {
    QString coordString = in.attributes().value("coordinates").toString();
    setPos(coordString.section(",",0,0).toDouble(),
           coordString.section(",",1,1).toDouble()); // TODO static utility function
    setHtml(in.readElementText());
    return in;
  }

  QXmlStreamWriter &TextItem::writeXml(QXmlStreamWriter &out) const
  {
    out.writeStartElement(xmlClassName());
    out.writeAttribute("coordinates", QString::number(pos().x()) +"," + QString::number(pos().y())); // TODO static utility function for this
    out.writeCDATA(toHtml());
    out.writeEndElement();
    return out;
  }

  QString TextItem::xmlClassName() {
    return "textItem";
  }

  void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    if (event->modifiers() != Qt::NoModifier) return;
    event->accept();
  }

  void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->modifiers() != Qt::NoModifier) return;
    if (event->button() != Qt::NoButton && event->button() != Qt::LeftButton) return;
    event->accept();
    QPointF newPosition = pos() + event->scenePos() - event->lastScenePos() ;
    MolScene *sc = dynamic_cast<MolScene*>(scene());
    if (sc) newPosition = sc->snapToGrid(newPosition);
    Commands::MoveItem::absolute(this, newPosition, tr("Move text item"))->execute();
  }

  class TextEditingUndoCommand : public Commands::ItemCommand<TextItem, TextEditingUndoCommand>  { // TODO unit test
    TextItem *item;
    QTextDocument *originalContent;
  public:
    void redo() override {
      QTextDocument *old = item->document();
      QObject *parent = old->parent();
      old->setParent(nullptr);
      originalContent->setParent(parent);
      item->setDocument(originalContent);
      originalContent = old;
    }
    explicit TextEditingUndoCommand(TextItem *item, const QString& text )
      : ItemCommand(text),
        item(item), originalContent(item->document()->clone()) {}
    ~TextEditingUndoCommand() { delete originalContent; }
    TextItem* getItem() const override { return item; }
  };

  void TextItem::focusInEvent(QFocusEvent *event) {
    if (!d->initialFill)
      (new TextEditingUndoCommand(this, tr("Edit text")))->execute();
    d->initialFill = false;
    d->hovering = false;
    QGraphicsTextItem::focusInEvent(event);
    event->accept();
  }

  void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QGraphicsTextItem::paint(painter, option, widget);
    if (d->hovering) {
      painter->save();
      painter->setPen(QPen(Qt::blue, 1, Qt::DotLine));
      painter->drawPath(shape());
      painter->restore();
    }
  }

  void TextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    d->hovering = !hasFocus();
    QGraphicsTextItem::hoverEnterEvent(event);
    update();
  }

  void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    d->hovering = false;
    QGraphicsTextItem::hoverLeaveEvent(event);
    update();
  }
} // namespace Molsketch
