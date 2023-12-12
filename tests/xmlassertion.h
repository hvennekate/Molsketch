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
#ifndef XMLASSERTION_H
#define XMLASSERTION_H

#include <QScopedPointer>
#include <QString>
#include <xmlobjectinterface.h>
#include "assertionbase.h"

class XmlAssertionBasePrivate;
class XmlAssertionBase : private AssertionBase {
  Q_DECLARE_PRIVATE(XmlAssertionBase)
  QScopedPointer<XmlAssertionBasePrivate> d_ptr;
protected:
  XmlAssertionBase(const QString &previousMessages);
  void printStackTraceAndThrow(const QString &message) const;
  QString previousMessages() const;
public:
  virtual ~XmlAssertionBase();
private:
  Q_DISABLE_COPY(XmlAssertionBase)
};

class StringListAssertionPrivate;
class StringListAssertion : private XmlAssertionBase {
  Q_DECLARE_PRIVATE(StringListAssertion)
  QScopedPointer<StringListAssertionPrivate> d_ptr;
public:
  StringListAssertion(const QStringList &strings, const QString &previousMessages = QString());
  const StringListAssertion *exactly(const QStringList &expected) const;
  const StringListAssertion *exactlyInAnyOrder(const QStringList &expected) const;
  ~StringListAssertion();
private:
  Q_DISABLE_COPY(StringListAssertion)
};

class XmlNode;
class XmlNodesAssertionPrivate;
class XmlNodesAssertion : private XmlAssertionBase {
  Q_DECLARE_PRIVATE(XmlNodesAssertion)
  QScopedPointer<XmlNodesAssertionPrivate> d_ptr;
public:
  ~XmlNodesAssertion();
  const XmlNodesAssertion *times(int expectedCount) const;
  const XmlNodesAssertion *exactlyOne() const;
  void none() const;
  const XmlNodesAssertion *index(int i) const;
  const XmlNodesAssertion *withNoAttribute(const QString &attribute) const;
  const StringListAssertion *haveAttribute(const QString &attribute) const;
  const StringListAssertion *haveTexts() const;
private:
  XmlNodesAssertion(const QList<XmlNode> &, const QString &message);
  friend class XmlAssertion;
  Q_DISABLE_COPY(XmlNodesAssertion)
};

class XmlAssertionPrivate;
class XmlAssertion : private XmlAssertionBase {
  Q_DECLARE_PRIVATE(XmlAssertion)
  QScopedPointer<XmlAssertionPrivate> d_ptr;
public:
  ~XmlAssertion();
  // TODO add method for description
  static const XmlAssertion *assertThat(const QString& xml);
  static const XmlAssertion *assertThat(const Molsketch::XmlObjectInterface &object);
  const XmlNodesAssertion *hasNodes(const QString& xQuery) const;
  const XmlNodesAssertion *hasParentOf(const QString& xQuery) const;
private:
  XmlAssertion(const QString& xml);
  QStringList getStringResults();
  Q_DISABLE_COPY(XmlAssertion)
};

namespace XmlAssert {
  QString formatXml(const QString &xml);
  const XmlAssertion *assertThat(const QString& xml);
  const XmlAssertion *assertThat(const Molsketch::XmlObjectInterface& object);
}

#endif // XMLASSERTION_H
