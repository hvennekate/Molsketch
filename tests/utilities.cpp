/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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

#include "utilities.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QTableView>
#include <QXmlStreamReader>
#include <QMenuBar>

void mouseMoveEvent(QWidget *widget, Qt::MouseButton button, Qt::KeyboardModifiers stateKey, QPoint pos, int delay) {
    QTEST_ASSERT(widget);

    if (pos.isNull())
        pos = widget->rect().center();

    if (delay > 0)
        QTest::qWait(delay);

    QTEST_ASSERT(stateKey == 0 || stateKey & Qt::KeyboardModifierMask);

    stateKey &= static_cast<unsigned int>(Qt::KeyboardModifierMask);

    QMouseEvent me(QEvent::MouseMove, pos, widget->mapToGlobal(pos), button, Qt::NoButton, stateKey);
    QSpontaneKeyEvent::setSpontaneous(&me);
    if (!qApp->notify(widget, &me)) {
        QString warning = QString::fromLatin1("Mouse event \"MouseMove\" not accepted by receiving widget");
        QTest::qWarn(warning.toLatin1().data());
    }
}

QWidget* focusTableCell(QTableView *table, int row, int column) {
  QPoint position(table->columnViewportPosition(column) + .5 * table->columnWidth(column),
                  table->rowViewportPosition(row) + .5 * table->rowHeight(row));
  leftMouseClick(table->viewport(), position);
  doubleClick(table->viewport(), position);
  return table->viewport()->focusWidget();
}

void enterDataIntoCell(QTableView *table, const QString& data, int row, int column) {
  QWidget* editor = focusTableCell(table, row, column);
  QSM_ASSERT(QString("No editor for cell %1, %2").arg(row).arg(column), editor);
  if (editor) {
    QTest::keyClicks(editor, data);
  }
  focusTableCell(table, 0, 0);
}

void enterTextIntoInputWidget(QLineEdit *editor, const QString& text, int position) {
  leftMouseClick(editor);
  QTest::keyClick(editor, Qt::Key_Home);
  for (int i = 0 ; i < position ; ++i)
    QTest::keyClick(editor, Qt::Key_Right);
  QTest::keyClicks(editor, text);
}



void assertTrue(bool input, QString message) {
  TSM_ASSERT(message.toStdString().data(), input);
  if (!input) throw nullptr;
}

void clickCheckBox(QCheckBox *checkBox) {
  auto minimumSize = checkBox->minimumSizeHint();
  leftMouseClick(checkBox, {minimumSize.width()/2, minimumSize.height()/2});
}

int xmlElementCount(const QString& xml, const QString& element) {
  int count = 0;
  QXmlStreamReader reader(xml);
  while (findNextElement(reader, element))
    ++count;
  return count;
}

bool findNextElement(QXmlStreamReader& reader, const QString& element) {
  while (!reader.atEnd())
    if (QXmlStreamReader::StartElement == reader.readNext() && reader.name() == element)
      return true;
  return false;
}

QPolygonF getPointsFromXml(const QXmlStreamReader &reader) {
  QStringRef localValue = reader.attributes().value("points");
  QVector<QStringRef> pointsText = localValue.split(" ", Qt::SkipEmptyParts);
  QPolygonF points;
  for (QStringRef pointText : pointsText) {
    QVector<QStringRef> coordsText = pointText.split(",");
    QSM_ASSERT_EQUALS(pointText.toString(), coordsText.size(),2);
    points << QPointF(coordsText[0].toDouble(), coordsText[1].toDouble());
  }
  return points;
}

QXmlStreamAttributes getAttributesOfParentElement(QXmlStreamReader& reader, const QString &element) {
  QXmlStreamAttributes parentAttributes;
  while (!reader.atEnd()) {
    if (reader.name() == element) return parentAttributes;
    parentAttributes = reader.attributes();
    while (!reader.atEnd() && QXmlStreamReader::StartElement != reader.readNext());
  }
  return QXmlStreamAttributes();
}

template<typename T, QString (T::*FP)() const>
T *findItem(const QList<T*> &items, const QString& text) {
 for (auto item : items)
   if ((item->*FP)().remove(QRegExp("&(?!&)")) == text)
     return item;
 TS_FAIL("Could not find item by the name of " + text);
 return nullptr;
}

void clickMenuEntry(const QStringList &names, QMainWindow *mainWindow) {
  auto menuBar = mainWindow->menuBar();
  auto menu = findItem<QMenu, &QMenu::title>(menuBar->findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly), names.first());
  leftMouseClick(menuBar, menuBar->actionGeometry(menu->menuAction()).center());

  for (auto subMenuName : names.mid(1, names.size() -2)) {
    qFatal("Clicking on submenus is not yet implemented!"); // TODO
  }
  auto action = findItem<QAction, &QAction::text>(menu->actions(), names.last());
  leftMouseClick(menu, menu->actionGeometry(action).center());
}

void leftMouseClick(QWidget *w, QPoint p) {
  QTest::mouseClick(w, Qt::LeftButton, Qt::KeyboardModifiers(), p);
}

void leftMouseClick(QWindow *w, QPoint p) {
  QTest::mouseClick(w, Qt::LeftButton, Qt::KeyboardModifiers(), p);
}

void doubleClick(QWidget *w, QPoint p) {
  QTest::mouseDClick(w, Qt::LeftButton, Qt::KeyboardModifiers(), p);
}

void doubleClick(QWindow *w, QPoint p) {
  QTest::mouseDClick(w, Qt::LeftButton, Qt::KeyboardModifiers(), p);
}

QDebug operator <<(QDebug debug, const std::string &string) {
  return debug << QString::fromStdString(string);
}
