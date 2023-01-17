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
#include "xmlassertion.h"
#include <execinfo.h>
#ifdef MSKTEST_STACKTRACE
#include <stacktrace.hpp>
#endif
#include <iostream>
#include <QTextStream>
#include <QXmlStreamReader>

#include <QDebug>

// TODO make the stack trace facility an abstract parent
const int MAX_STACK_TRACE_DEPTH = 15;

QString XmlAssert::formatXml(const QString &xml) {
  QXmlStreamReader reader(xml);
  QString formattedXml;
  QXmlStreamWriter writer(&formattedXml);
  writer.setAutoFormatting(true);

  while (!reader.atEnd()) {
      reader.readNext();
      if (!reader.isWhitespace()) {
          writer.writeCurrentToken(reader);
      }
  }
  return formattedXml;
}

struct XmlAssertionBasePrivate {
  QString previousMessages;
};

XmlAssertionBase::XmlAssertionBase(const QString &previousMessages)
  : d_ptr(new XmlAssertionBasePrivate) {
  Q_D(XmlAssertionBase);
  d->previousMessages = previousMessages;
}

void XmlAssertionBase::printStackTraceAndThrow(const QString& message) const {
  Q_D(const XmlAssertionBase);
  auto completeMessage = message + "\n" + d->previousMessages;
  QTextStream out(stdout, QIODevice::WriteOnly);
  out << completeMessage;
#ifdef MSKTEST_STACKTRACE
  std::cout << boost::stacktrace::stacktrace();
#endif
  throw completeMessage;
}

QString XmlAssertionBase::previousMessages() const {
  Q_D(const XmlAssertionBase);
  return d->previousMessages;
}

XmlAssertionBase::~XmlAssertionBase() {}

struct StringListAssertionPrivate {
  QStringList strings;
};

StringListAssertion::StringListAssertion(const QStringList &strings, const QString &previousMessages)
  : XmlAssertionBase(previousMessages),
    d_ptr(new StringListAssertionPrivate)
{
  Q_D(StringListAssertion);
  d->strings = strings;
}

QString formatStringList(QStringList list) {
  return "[" + list
      .replaceInStrings(QRegExp("^"), "\"")
      .replaceInStrings(QRegExp("$"), "\"")
      .join(", ") + "]";
}

const StringListAssertion *StringListAssertion::exactly(const QStringList &expected) const {
  Q_D(const StringListAssertion);
  if (d->strings.size() != expected.size())
    printStackTraceAndThrow(QString("Number of expected and acutal elements differ!")
                               + "\nExpected values: " + formatStringList(expected)
                               + "\nActual values:   " + formatStringList(d->strings));
  for (int i = 0 ; i < d->strings.size(); ++i)
    if (d->strings[i] != expected[i])
      printStackTraceAndThrow("Actual and expected elements differ at index=" + QString::number(i)
                                 + "\nExpected values: " + formatStringList(expected)
                                 + "\nActual values:   " + formatStringList(d->strings));
  return this;
}

const StringListAssertion *StringListAssertion::exactlyInAnyOrder(const QStringList &expectedValues) const {
  Q_D(const StringListAssertion);
  auto actualValues = d->strings;
  for (const QString& actual : actualValues)
    if (!expectedValues.contains(actual))
      printStackTraceAndThrow("Unexpected value found: \"" + actual + "\""
                                 + "\nExpected values: " + formatStringList(expectedValues)
                                 + "\nActual values:   " + formatStringList(actualValues));
  for (const QString& expected : expectedValues)
    if (!actualValues.contains(expected))
      printStackTraceAndThrow("Expected value not found: \"" + expected + "\""
                                 + "\nExpected values: " + formatStringList(expectedValues)
                                 + "\nActual values:   " + formatStringList(actualValues));
  return this;
}

StringListAssertion::~StringListAssertion() {}

struct XmlNode {
    QXmlStreamAttributes attributes;
    QString text;
    QStringRef name;
    QList<size_t> location;
    QString locationAsString() const {
      if (location.empty()) return "";
      auto result = QString::number(location.at(0));
      for (auto value : location.mid(1))
        result += "/" + QString::number(value);
      return result;
    }
};

struct XmlNodesAssertionPrivate {
  QList<XmlNode> nodes;
};

XmlNodesAssertion::XmlNodesAssertion(const QList<XmlNode> &nodes, const QString &message)
  : XmlAssertionBase(message),
    d_ptr(new XmlNodesAssertionPrivate)
{
  Q_D(XmlNodesAssertion);
  d->nodes = nodes;
}

XmlNodesAssertion::~XmlNodesAssertion() {}

const XmlNodesAssertion *XmlNodesAssertion::times(int expectedCount) const {
  Q_D(const XmlNodesAssertion);
  if (d->nodes.size() != expectedCount)
    printStackTraceAndThrow(
          QString("Expected exactly %1 results from query. Actual count: %2.")
          .arg(expectedCount)
          .arg(d->nodes.size()));
  return this;
}

const XmlNodesAssertion *XmlNodesAssertion::exactlyOne() const {
  return times(1);
}

void XmlNodesAssertion::none() const {
  times(0);
}

const XmlNodesAssertion *XmlNodesAssertion::index(int i) const {
  Q_D(const XmlNodesAssertion);
  if (d->nodes.size() <= i)
    printStackTraceAndThrow(QString("Requested entry %1 of list with %2 entries").arg(i).arg(d->nodes.size()));
  return new XmlNodesAssertion(d->nodes.mid(i,1), QString("Node %1\n").arg(i) + previousMessages());
}

const XmlNodesAssertion *XmlNodesAssertion::withNoAttribute(const QString &attribute) const {
  Q_D(const XmlNodesAssertion);
  QStringList violatingNodes;
  for (auto node : d->nodes)
    if (node.attributes.hasAttribute(attribute))
      violatingNodes << node.name.toString();
  if (!violatingNodes.isEmpty())
    printStackTraceAndThrow(QString("Expected nodes not to have attribute '%1', but the following did: %2")
                            .arg(attribute)
                            .arg(violatingNodes.join(", ")));
  return this;
}

const StringListAssertion *XmlNodesAssertion::haveAttribute(const QString &attribute) const {
  Q_D(const XmlNodesAssertion);
  QStringList values;
  QStringList nodesWithoutAttribute;
  for (auto node : d->nodes) {
    if (!node.attributes.hasAttribute(attribute)) nodesWithoutAttribute << node.locationAsString();
    values << node.attributes.value(attribute).toString();
  }
  if (!nodesWithoutAttribute.empty())
    printStackTraceAndThrow(QString("Expected nodes to have attribute %1, but the following nodes did not: %2")
                            .arg(attribute)
                            .arg(nodesWithoutAttribute.join(", ")));
  return new StringListAssertion(values, "for attribute '" + attribute +"'\n" + previousMessages());
}

const StringListAssertion *XmlNodesAssertion::haveTexts() const {
  Q_D(const XmlNodesAssertion);
  QStringList values;
  std::transform(d->nodes.cbegin(), d->nodes.cend(), std::back_inserter(values),
                 [] (const XmlNode &node) { return node.text; });
  return new StringListAssertion(values, "for node text\n" + previousMessages());
}

struct XmlAssertionPrivate {
  QMultiMap<QString, XmlNode> nodes;
  XmlAssertionPrivate(const QString &xml)
  {
    QXmlStreamReader in(xml);

    QStringList currentPath;
    QList<size_t> location{0};
    while (!in.atEnd() && in.readNext() != QXmlStreamReader::Invalid) {
      if (in.tokenType() == QXmlStreamReader::StartElement) {
        XmlNode node;
        node.attributes = in.attributes();
        node.location = QList(location);
        node.name = in.name();
        location << 0;
        currentPath += in.name().toString();
        nodes.insert(currentPath.join("/"), node);
      }
      if (in.tokenType() == QXmlStreamReader::EndElement) {
        location.removeLast();
        location.last() += 1;
        currentPath.removeLast();
      }
      if (in.tokenType() == QXmlStreamReader::Characters
          && !in.isWhitespace()) {
        auto key = currentPath.join("/");
        auto currentNode = nodes.take(key);
        currentNode.text += in.text();
        nodes.insert(key, currentNode);
      }
    }
  }
};

XmlAssertion::~XmlAssertion() {}

const XmlAssertion *XmlAssertion::assertThat(const QString &xml) {
  return new XmlAssertion(xml);
}

const XmlAssertion *XmlAssertion::assertThat(const Molsketch::XmlObjectInterface &object) {
  QString xml;
  QXmlStreamWriter out(&xml);
  object.writeXml(out);
  return new XmlAssertion(xml);
}

const XmlNodesAssertion *XmlAssertion::hasNodes(const QString &xQuery) const {
  Q_D(const XmlAssertion);
  auto nodes = d->nodes.values(xQuery);
  std::reverse(nodes.begin(), nodes.end());
  return new XmlNodesAssertion(nodes, "for path '" + xQuery + "'\n" + previousMessages());
}

const XmlNodesAssertion *XmlAssertion::hasParentOf(const QString &xQuery) const {
  Q_D(const XmlAssertion);
  auto childNodes = d->nodes.values(xQuery);
  QList<QList<size_t>> locations;
  for (auto childNode : childNodes) locations << childNode.location.mid(0, childNode.location.size() - 1);
  auto parts = xQuery.split("/");
  auto parentQuery = QStringList(parts.mid(0, parts.size() -1)).join("/");
  auto potentialParentNodes = d->nodes.values(parentQuery);
  QList<XmlNode> parents;
  for (auto potentialParent : potentialParentNodes)
    if (locations.contains(potentialParent.location))
      parents += potentialParent;
  std::reverse(parents.begin(), parents.end());
  return new XmlNodesAssertion(parents, "parents of nodes with path '" + xQuery + "'\n'" + previousMessages());
}

XmlAssertion::XmlAssertion(const QString &xml)
  : XmlAssertionBase("with XML:\n" + XmlAssert::formatXml(xml)),
    d_ptr(new XmlAssertionPrivate(xml))
{}

const XmlAssertion *XmlAssert::assertThat(const QString &xml) { return XmlAssertion::assertThat(xml); }

const XmlAssertion *XmlAssert::assertThat(const Molsketch::XmlObjectInterface &object) {
  return XmlAssertion::assertThat(object);
}
