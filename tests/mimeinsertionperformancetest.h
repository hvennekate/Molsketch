/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate                               *
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
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <molscene.h>
#include "utilities.h"

using namespace Molsketch;

const QString MOLECULE_XML("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                           "<molecule name=\"\">"
                           "    <atomArray>"
                           "        <atom id=\"a1\" elementType=\"C\" hydrogenCount=\"2\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"28.2845,-79.6078\"/>"
                           "        <atom id=\"a2\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,40.3922\"/>"
                           "        <atom id=\"a3\" elementType=\"C\" hydrogenCount=\"2\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"8.2845,-114.249\"/>"
                           "        <atom id=\"a4\" elementType=\"C\" hydrogenCount=\"2\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"8.2845,-44.9669\"/>"
                           "        <atom id=\"a5\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-273.644,20.3922\"/>"
                           "        <atom id=\"a6\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-342.925,20.3922\"/>"
                           "        <atom id=\"a7\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-31.7155,-114.249\"/>"
                           "        <atom id=\"a8\" elementType=\"C\" hydrogenCount=\"2\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-31.7155,-44.9669\"/>"
                           "        <atom id=\"a9\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-273.644,-19.6078\"/>"
                           "        <atom id=\"a10\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-342.925,-19.6078\"/>"
                           "        <atom id=\"a11\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-111.716,-114.249\"/>"
                           "        <atom id=\"a12\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-111.716,-44.9669\"/>"
                           "        <atom id=\"a13\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-151.716,-114.249\"/>"
                           "        <atom id=\"a14\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-151.716,-44.9669\"/>"
                           "        <atom id=\"a15\" elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-11.3235\"/>"
                           "        <atom id=\"a16\" elementType=\"C\" hydrogenCount=\"2\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-147.892\"/>"
                           "        <atom id=\"a17\" elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-51.7155,-79.6078\"/>"
                           "        <atom id=\"a18\" elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-91.7155,-79.6078\"/>"
                           "        <atom id=\"a19\" elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-171.716,-79.6078\"/>"
                           "        <atom id=\"a20\" elementType=\"C\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,-39.6078\"/>"
                           "        <atom id=\"a21\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-51.3235\"/>"
                           "        <atom id=\"a22\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-107.892\"/>"
                           "        <atom id=\"a23\" elementType=\"C\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-211.716,-79.6078\"/>"
                           "        <atom id=\"a24\" elementType=\"N\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,28.6762\"/>"
                           "        <atom id=\"a25\" elementType=\"N\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-268.284,-79.6078\"/>"
                           "        <atom id=\"a26\" elementType=\"O\" hydrogenCount=\"1\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-274.641,-167.892\"/>"
                           "        <atom id=\"a27\" elementType=\"O\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-348.284,-79.6078\"/>"
                           "        <atom id=\"a28\" elementType=\"O\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,-119.608\"/>"
                           "        <atom id=\"a29\" elementType=\"S\" hydrogenCount=\"0\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,-79.6078\"/>"
                           "    </atomArray>"
                           "    <bondArray>"
                           "        <bond atomRefs2=\"a1 a3\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"28.2845,-79.6078;8.2845,-114.249\"/>"
                           "        <bond atomRefs2=\"a1 a4\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"28.2845,-79.6078;8.2845,-44.9669\"/>"
                           "        <bond atomRefs2=\"a2 a5\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,40.3922;-273.644,20.3922\"/>"
                           "        <bond atomRefs2=\"a2 a6\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,40.3922;-342.925,20.3922\"/>"
                           "        <bond atomRefs2=\"a3 a7\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"8.2845,-114.249;-31.7155,-114.249\"/>"
                           "        <bond atomRefs2=\"a4 a8\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"8.2845,-44.9669;-31.7155,-44.9669\"/>"
                           "        <bond atomRefs2=\"a5 a9\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-273.644,20.3922;-273.644,-19.6078\"/>"
                           "        <bond atomRefs2=\"a10 a6\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-342.925,-19.6078;-342.925,20.3922\"/>"
                           "        <bond atomRefs2=\"a7 a17\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-31.7155,-114.249;-51.7155,-79.6078\"/>"
                           "        <bond atomRefs2=\"a8 a17\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-31.7155,-44.9669;-51.7155,-79.6078\"/>"
                           "        <bond atomRefs2=\"a9 a20\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-273.644,-19.6078;-308.284,-39.6078\"/>"
                           "        <bond atomRefs2=\"a10 a20\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-342.925,-19.6078;-308.284,-39.6078\"/>"
                           "        <bond atomRefs2=\"a11 a13\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-111.716,-114.249;-151.716,-114.249\"/>"
                           "        <bond atomRefs2=\"a18 a11\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-91.7155,-79.6078;-111.716,-114.249\"/>"
                           "        <bond atomRefs2=\"a14 a12\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-151.716,-44.9669;-111.716,-44.9669\"/>"
                           "        <bond atomRefs2=\"a12 a18\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-111.716,-44.9669;-91.7155,-79.6078\"/>"
                           "        <bond atomRefs2=\"a13 a19\" type=\"22\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-151.716,-114.249;-171.716,-79.6078\"/>"
                           "        <bond atomRefs2=\"a14 a19\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-151.716,-44.9669;-171.716,-79.6078\"/>"
                           "        <bond atomRefs2=\"a15 a21\" type=\"11\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-11.3235;-240,-51.3235\"/>"
                           "        <bond atomRefs2=\"a15 a24\" type=\"30\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-11.3235;-240,28.6762\"/>"
                           "        <bond atomRefs2=\"a16 a22\" type=\"11\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-147.892;-240,-107.892\"/>"
                           "        <bond atomRefs2=\"a16 a26\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-147.892;-274.641,-167.892\"/>"
                           "        <bond atomRefs2=\"a17 a18\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-51.7155,-79.6078;-91.7155,-79.6078\"/>"
                           "        <bond atomRefs2=\"a19 a23\" type=\"11\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-171.716,-79.6078;-211.716,-79.6078\"/>"
                           "        <bond atomRefs2=\"a20 a29\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,-39.6078;-308.284,-79.6078\"/>"
                           "        <bond atomRefs2=\"a21 a23\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-51.3235;-211.716,-79.6078\"/>"
                           "        <bond atomRefs2=\"a21 a25\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-51.3235;-268.284,-79.6078\"/>"
                           "        <bond atomRefs2=\"a22 a23\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-107.892;-211.716,-79.6078\"/>"
                           "        <bond atomRefs2=\"a22 a25\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-240,-107.892;-268.284,-79.6078\"/>"
                           "        <bond atomRefs2=\"a25 a29\" type=\"10\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-268.284,-79.6078;-308.284,-79.6078\"/>"
                           "        <bond atomRefs2=\"a27 a29\" type=\"23\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-348.284,-79.6078;-308.284,-79.6078\"/>"
                           "        <bond atomRefs2=\"a28 a29\" type=\"23\" colorR=\"0\" colorG=\"0\" colorB=\"0\" scalingParameter=\"1\" coordinates=\"-308.284,-119.608;-308.284,-79.6078\"/>"
                           "    </bondArray>"
                           "</molecule>\n");

struct MolSceneForTesting : public MolScene {
public:
  void sendDragEnterEvent(QGraphicsSceneDragDropEvent *event) {
    dragEnterEvent(event);
  }
};


class MimeInsertionPerformanceTest : public CxxTest::TestSuite {
public:
  void testInsertingComplexSampleMolecule() {
    auto scene = new MolSceneForTesting;
    auto event = new QGraphicsSceneDragDropEvent(QGraphicsSceneDragDropEvent::GraphicsSceneDragEnter);
    auto mimeData = new QMimeData;
    mimeData->setData(scene->mimeType(), MOLECULE_XML.toUtf8());
    event->setMimeData(mimeData);
    event->setProposedAction(Qt::CopyAction);
    QTime stopwatch;
    stopwatch.start();
    scene->sendDragEnterEvent(event);
    TSM_ASSERT_LESS_THAN("Inserting complex molecule took too long!", stopwatch.elapsed(), 500); // TODO limit should be 100
  }
};
