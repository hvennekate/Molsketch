/***************************************************************************
 *   Copyright (C) 2021 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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
#ifndef LISTASSERTION_H
#define LISTASSERTION_H

#ifdef MSKTEST_STACKTRACE
#include <stacktrace.hpp>
#endif

#include <QDebug>
#include<QList>


template <class T>
class ListAssertion
{
  QList<T> list;
  QString additionalDescription;

  QString printList() {
    QString string;
    QDebug out(&string);
    out.setAutoInsertSpaces(false);
    out << "List content: " << list;
    return string;
  }
public:
  ListAssertion(const QList<T> &list) : list(list) {}

  template<class UnaryPredicate>
  ListAssertion<T> &filteredOn(UnaryPredicate predicate) {
    QList<T> newList;
    std::copy_if(list.cbegin(), list.cend(), std::back_inserter(newList), predicate);
    newList.swap(list);
    return *this;
  }

  template<class U>
  ListAssertion<U> selectingType() {
    QList<T> filteredList;
    std::copy_if(list.cbegin(), list.cend(), std::back_inserter(filteredList),
                 [] (T t) { return dynamic_cast<U>(t); });
    QList<U> newList;
    std::transform(filteredList.cbegin(), filteredList.cend(), std::back_inserter(newList),
                   [] (T t) { return dynamic_cast<U>(t); });
    return ListAssertion<U>(newList);
  }

  ListAssertion<T> as(const QString &message) {
    additionalDescription = message;
    return *this;
  }

  ListAssertion<T> &hasSize(const int &expectedCount) {
    if (list.size() != expectedCount) {
      auto message = QString("List size not as expected! Expected: %1. Actual: %2\n%3")
          .arg(expectedCount)
          .arg(list.size())
          .arg(printList());
      QTextStream out(stdout, QIODevice::WriteOnly);
      out << message << endl;
#ifdef MSKTEST_STACKTRACE
      std::cout << boost::stacktrace::stacktrace();
#endif
      throw message;
    }
    return *this;
  }

  ListAssertion<T> &isEmpty() {
    return hasSize(0);
  }
};

template<class T>
static ListAssertion<T> assertThat(const QList<T> &list) {
  return ListAssertion<T>(list);
}
#endif // LISTASSERTION_H
