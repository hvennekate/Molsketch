/***************************************************************************
 *   Copyright (C) 2017 Hendrik Vennekate                                  *
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
#ifndef SIGNALCOUNTER_H
#define SIGNALCOUNTER_H

#include <tuple>
#include <functional>
#include <QList>
#include <utilities.h>
#include <QDebug>
#include <QObject>
#include <QModelIndex>

template<typename ... Types>
class SignalCounter
{
private:
  using FirstType = typename std::tuple_element<0, std::tuple<Types...>>::type;
  using Content = typename std::conditional<(sizeof...(Types) == 1), FirstType, std::tuple<Types...>>::type;
  QList<Content> payloads;
public:
  std::function<void(Types...)> callback;
  Content getLatestPayload() const {Q_ASSERT(!payloads.empty()); return payloads.last();}
  int count() const { return payloads.size(); }
  Content getPayload(int index) { Q_ASSERT(index >= 0 && index < payloads.size()) ; return payloads[index]; }
  template<typename T = Content>
  typename std::enable_if<!std::is_same<T, FirstType>::value>::type addPayload(Types... newPayload) {
    payloads << std::make_tuple(newPayload...);
    if (callback) callback(newPayload...);
  }
  template<typename T = Content>
  typename std::enable_if<std::is_same<T, FirstType>::value>::type addPayload(Types... newPayload) {
    payloads << std::get<0>(std::make_tuple(newPayload...));
    if (callback) callback(newPayload...);
  }

  void assertPayloads(const QList<Content>& expected) { QS_ASSERT_EQUALS(expected, payloads); }
};

template<typename ... Types>
QDebug operator<<(QDebug debug, const std::tuple<Types...>& tuple) {
  debug << "Tuple [";
  printTupel(tuple, debug);
  return debug << "]";
}

template<std::size_t I = 0, typename... TupelTypes>
typename std::enable_if<I == sizeof...(TupelTypes)>::type
  printTupel(const std::tuple<TupelTypes...>&, QDebug&) {}

template<std::size_t I = 0, typename... TupelTypes>
typename std::enable_if<I < sizeof...(TupelTypes)>::type
  printTupel(const std::tuple<TupelTypes...>& tupel, QDebug& debug)
  {
    debug << std::get<I>(tupel);
    if (I + 1 < sizeof...(TupelTypes)) debug << ", ";
    printTupel<I + 1, TupelTypes...>(tupel, debug);
  }

class QRealSignalCounter : public QObject, public SignalCounter<qreal> {
  Q_OBJECT
public:
  explicit QRealSignalCounter(QObject *parent = 0);
public slots:
  void record(const qreal &value);
};

class BoolSignalCounter : public QObject, public SignalCounter<bool> {
  Q_OBJECT
public:
  explicit BoolSignalCounter(QObject *parent = 0);
public slots:
  void record(const bool& value);
};

class ColorSignalCounter : public QObject, public SignalCounter<QColor> {
  Q_OBJECT
public:
  explicit ColorSignalCounter(QObject *parent = 0);
public slots:
  void record(const QColor&value);
};

class FontSignalCounter : public QObject, public SignalCounter<QFont> {
  Q_OBJECT
public:
  explicit FontSignalCounter(QObject *parent = 0);
public slots:
  void record(const QFont&value);
};

class StringListSignalCounter : public QObject, public SignalCounter<QStringList> {
  Q_OBJECT
public:
  explicit StringListSignalCounter(QObject *parent = 0);
public slots:
  void record(const QStringList&value);
};

class StringDoubleMapSignalCounter : public QObject, public SignalCounter<QMap<QString, qreal>> {
  Q_OBJECT
public:
  explicit StringDoubleMapSignalCounter(QObject *parent = 0);
public slots:
  void record(const QMap<QString, qreal> &value);
};

class StringIntDoubleMapSignalCounter : public QObject, public SignalCounter<QMap<std::pair<QString, int>, qreal>> {
  Q_OBJECT
public:
  explicit StringIntDoubleMapSignalCounter(QObject *parent = 0);
public slots:
  void record(const QMap<std::pair<QString, int>, qreal> &value);
};

class ModelIndexSignalCounter : public QObject, public SignalCounter<QModelIndex, int, int>
{
  Q_OBJECT
public:
  explicit ModelIndexSignalCounter(QObject *parent = 0);
public slots:
  void record(const QModelIndex& index, int start, int end);
};

#endif // SIGNALCOUNTER_H
