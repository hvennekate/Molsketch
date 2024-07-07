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

#include <cxxtest/TestSuite.h>
#include "utilities.h"
#include <QXmlStreamReader>
#include <abstractxmlobject.h>
#include <arrow.h>
#include <atom.h>
#include <bond.h>
#include <boundingboxlinker.h>
#include <frame.h>
#include <lonepair.h>
#include <molecule.h>
#include <molscene.h>
#include <radicalelectron.h>
#include <textitem.h>
#include "mocks.h"

using namespace Molsketch;

struct XmlObjectInterfaceForTesting : public XmlObjectInterface {
  QXmlStreamReader& readXml(QXmlStreamReader &in){
    QS_ASSERT_EQUALS(in.name().toString(), "innerTestElement");
    TS_ASSERT_EQUALS(in.readNext(), QXmlStreamReader::EndElement);
    return in;
  }
  QXmlStreamWriter& writeXml(QXmlStreamWriter &out) const {
    out.writeEmptyElement("innerTestElement");
    return out;
  }
};

class AbstractXmlObjectForTesting : public abstractXmlObject {
public:

  typedef abstractXmlObject super;
  SUPER_MOCK_CONST(QList<const XmlObjectInterface*>, children, , )
  SUPER_MOCK(XmlObjectInterface*, produceChild, const QString& name COMMA const QXmlStreamAttributes& attr, name COMMA attr)
  VOID_SUPER_MOCK(readAttributes, const QXmlStreamAttributes& attr, attr)
  MOCK_CONST(QXmlStreamAttributes, xmlAttributes, , )
  VOID_SUPER_MOCK(afterReadFinalization, , )
  MOCK_CONST(QString, xmlName, , )
};

class AbstractXmlObjectUnitTest : public CxxTest::TestSuite {
  AbstractXmlObjectForTesting* testObject;
  QXmlStreamReader *reader;
  QXmlStreamWriter *writer;

  void prepareReader(const QString& input) {
    reader = new QXmlStreamReader(input);
    reader->readNext();
    reader->readNext();
  }

  void prepareWriter(QString *target) {
    writer = new QXmlStreamWriter(target);
  }

  static std::function<abstractXmlObject *(QString, QXmlStreamAttributes)> noChildrenToProduce() {
    return [](QString a, QXmlStreamAttributes b) { Q_UNUSED(a) Q_UNUSED(b)
      TSM_ASSERT("produceChild() should not be called if there are no children", false);
      return new AbstractXmlObjectForTesting; };
  }

  static std::function<QList<const XmlObjectInterface*>() > shouldNotCallChildren() {
    return [] {
      TSM_ASSERT("children() should not be called during read", false);
      return QList<const XmlObjectInterface*>(); };
  }

  static std::function<QXmlStreamAttributes()> shouldNotCallAttributes() {
    return [] () {
      TSM_ASSERT("xmlAtrributes() should not be called during read", false);
      return QXmlStreamAttributes(); };
  }

  struct ReadingTestSetup {
    AbstractXmlObjectForTesting *object;
    QXmlStreamAttributes readAttributes;
    int timesAttributesWereRead;
    int timesFinalized;
    int timesChildProduced;
    int desiredChildrenToProduce;

    ReadingTestSetup(AbstractXmlObjectForTesting* object)
      : object(object),
        timesAttributesWereRead(0),
        timesFinalized(0),
        timesChildProduced(0),
        desiredChildrenToProduce(0)
    {
      prepareDefaultScenario();
    }

    void prepareDefaultScenario() {
      object->produceChildCallback = noChildrenToProduce();
      object->childrenCallback = shouldNotCallChildren();
      object->xmlAttributesCallback = shouldNotCallAttributes();
      object->readAttributesCallback = [&](const QXmlStreamAttributes& attribs){
        readAttributes = attribs;
        ++timesAttributesWereRead;
        TSM_ASSERT("Finalization should happen after reading attributes", !timesFinalized); };
      object->afterReadFinalizationCallback = [&] (){
        TSM_ASSERT("Reading attributes should precede finalization", timesAttributesWereRead);
        ++timesFinalized; };
    }

    void assertTestResult(QList<QPair<QString, QString> > expectedAttributes = {}) {
      TSM_ASSERT_EQUALS("Arguments should be read exactly once", timesAttributesWereRead, 1);
      TSM_ASSERT_EQUALS("Finalization should occur exaclty once", timesFinalized, 1);

      TS_ASSERT_EQUALS(readAttributes.size(), expectedAttributes.size());
      for (auto attribute : expectedAttributes)
        TSM_ASSERT(("Did not contain " + attribute.first +"/" + attribute.second).toStdString(), readAttributes.contains(QXmlStreamAttribute(attribute.first, attribute.second)));
      TS_ASSERT_EQUALS(timesChildProduced, desiredChildrenToProduce);
    }

    void setupChildCallback(const QString& expectedName,
                            const QString& expectedType,
                            int desiredChildrenToProduce,
                            XmlObjectInterface* child) {
      this->desiredChildrenToProduce = desiredChildrenToProduce;
      object->produceChildCallback = [=](const QString& name, const QXmlStreamAttributes& attributes) {
        QS_ASSERT_EQUALS(name, expectedName);
        QS_ASSERT_EQUALS(attributes.value("type").toString(), expectedType);
        TSM_ASSERT("Should produce children before finalization", !timesFinalized);
        TSM_ASSERT_EQUALS("Should read attributes before reading childern", timesAttributesWereRead, 1);
        ++timesChildProduced;
        return child;
      };
    }
  };

  void prepareAttributes(AbstractXmlObjectForTesting* object, const QList<QPair<QString, QString> > & input)
  {
    QXmlStreamAttributes attributes;
    for (QPair<QString, QString> attribute : input)
      attributes.append(attribute.first, attribute.second);
    object->xmlAttributesDefaultReturn = attributes;
  }

public:
  void setUp() {
    testObject = new AbstractXmlObjectForTesting;
    reader = 0;
    writer = 0;
  }

  void tearDown() {
    if (reader) {
      TS_ASSERT(reader->isEndElement())
      QS_ASSERT_EQUALS(reader->name().toString(), "testElement")
    }
    delete testObject;
    delete reader;
    delete writer;
  }

  void testDefaultImplementations() {
    TS_ASSERT_EQUALS(testObject->produceChild("", QXmlStreamAttributes()), nullptr);
    QS_ASSERT_EQUALS(testObject->children(), QList<const XmlObjectInterface*>());
    TS_ASSERT_EQUALS(testObject->xmlAttributes(), QXmlStreamAttributes());
  }

  void testReadBareElement() {
    ReadingTestSetup setup(testObject);
    prepareReader("<testElement></testElement>");
    testObject->readXml(*reader);
    setup.assertTestResult();
  }

  void testReadElementWithAttributes() {
    ReadingTestSetup setup(testObject);

    prepareReader("<testElement testattribute=\"testvalue\" "
                  "otherattribute=\"othervalue\">"
                  "</testElement>");
    testObject->readXml(*reader);

    setup.assertTestResult({{"testattribute", "testvalue"}, {"otherattribute", "othervalue"}});
  }

  void testReadItemContent() {
    ReadingTestSetup setup(testObject);
    XmlObjectInterfaceForTesting content;
    setup.setupChildCallback("innerTestElement", "", 1, &content);
    prepareReader("<testElement><innerTestElement/></testElement>");
    testObject->readXml(*reader);
    setup.assertTestResult();
  }

  void testReadingChild() {
    AbstractXmlObjectForTesting child;
    ReadingTestSetup setup(testObject), childSetup(&child);
    setup.setupChildCallback("childObject", "childType", 1, &child);

    prepareReader("<testElement>"
                  "  <childObject type=\"childType\">"
                  "  </childObject>"
                  "</testElement>");
    testObject->readXml(*reader);

    setup.assertTestResult();
    childSetup.assertTestResult({{"type", "childType"}});
  }

  void testWritingPlainObject() {
    testObject->xmlNameDefaultReturn = "TestObjectForWriting";
    QString output;
    prepareWriter(&output);
    testObject->writeXml(*writer);
    QS_ASSERT_EQUALS(output, "<TestObjectForWriting/>");
  }

  void testWritingObjectWithAttribute() {
    testObject->xmlNameDefaultReturn = "TestObjectForWriting";
    prepareAttributes(testObject, {{"FirstTestAttribute", "firstValue"},
                      {"SecondTestAttribute", "secondValue"}});
    QString output;
    prepareWriter(&output);
    testObject->writeXml(*writer);
    QS_ASSERT_EQUALS(output, "<TestObjectForWriting "
                     "FirstTestAttribute=\"firstValue\" "
                     "SecondTestAttribute=\"secondValue\"/>");
  }

  void testWritingObjectWithChild() {
    AbstractXmlObjectForTesting child;
    child.xmlNameDefaultReturn = "child";

    testObject->xmlNameDefaultReturn = "parent";
    testObject->childrenCallback = [&]() { return QList<const XmlObjectInterface*>() << &child;};

    QString output;
    prepareWriter(&output);
    testObject->writeXml(*writer);
    QS_ASSERT_EQUALS(output, "<parent><child/></parent>");
  }

  void testWritingObjectWithChildAndAttributes() {
    AbstractXmlObjectForTesting child;
    child.xmlNameDefaultReturn = "child";
    prepareAttributes(&child, {{"cAttribute", "cValue"}});

    testObject->xmlNameDefaultReturn = "parent";
    testObject->childrenCallback = [&]() { return QList<const XmlObjectInterface*>() << &child;};
    prepareAttributes(testObject, {{"pAttribute", "pValue"}});

    QString output;
    prepareWriter(&output);
    testObject->writeXml(*writer);
    QS_ASSERT_EQUALS(output, "<parent pAttribute=\"pValue\"><child cAttribute=\"cValue\"/></parent>");
  }

  void testWritingItemContent() {
    XmlObjectInterfaceForTesting child;
    testObject->childrenCallback = [&]() { return QList<const XmlObjectInterface*>() << &child; };
    testObject->xmlNameDefaultReturn = "TestObject";

    QString output;
    prepareWriter(&output);
    testObject->writeXml(*writer);
    QS_ASSERT_EQUALS(output, "<TestObject><innerTestElement/></TestObject>");
  }

  void testRegisteredItemTypes() {
    TS_ASSERT(dynamic_cast<Atom*>(produceXmlObject(Atom::xmlClassName())));
    TS_ASSERT(dynamic_cast<Bond*>(produceXmlObject(Bond::xmlClassName())));
    TS_ASSERT(dynamic_cast<TextItem*>(produceXmlObject(TextItem::xmlClassName())));
    TS_ASSERT(dynamic_cast<BoundingBoxLinker*>(produceXmlObject(BoundingBoxLinker::xmlClassName())));
    TS_ASSERT(dynamic_cast<LonePair*>(produceXmlObject(LonePair::xmlClassName())));
    TS_ASSERT(dynamic_cast<MolScene*>(produceXmlObject(MolScene::xmlClassName())));
    TS_ASSERT(dynamic_cast<RadicalElectron*>(produceXmlObject(RadicalElectron::xmlClassName())));
    TS_ASSERT(dynamic_cast<Arrow*>(produceXmlObject(Arrow::xmlClassName())));
    TS_ASSERT(dynamic_cast<Frame*>(produceXmlObject(Frame::xmlClassName())));
    TS_ASSERT(dynamic_cast<Molecule*>(produceXmlObject(Molecule::xmlClassName())));
  }

  void testDoNotReadBeyondCurrentElement() {
    ReadingTestSetup setup(testObject);
    prepareReader("<testElement />\n<A />\n");
    testObject->readXml(*reader);
    QS_ASSERT_EQUALS(reader->name().toString(), "testElement");
    QS_ASSERT_EQUALS(reader->tokenType(), QXmlStreamReader::EndElement);
  }

  // TODO write test for reading legacy bonds: check cis/trans double bond & dashed single bond new
  // (legacy seems to be unrecoverable)
};
