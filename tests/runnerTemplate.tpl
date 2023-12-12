/***************************************************************************
 *   Copyright (C) 2023 by Hendrik Vennekate                               *
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
#include <cxxtest/XUnitPrinter.h>
#include <QApplication>
#include <QDebug>
#include "assertionbase.h"

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QByteArray localMsg = msg.toLocal8Bit();
  switch (type) {
    case QtDebugMsg:
      fprintf(stdout, "      Debug: [%s:%u, %s] %s\n", context.file, context.line, context.function, localMsg.constData());
      break;
    case QtWarningMsg:
      fprintf(stdout, "      Warning: [%s:%u, %s] %s\n", context.file, context.line, context.function, localMsg.constData());
      break;
    case QtCriticalMsg:
      fprintf(stdout, "      Critical: [%s:%u, %s] %s\n", context.file, context.line, context.function, localMsg.constData());
      break;
    case QtFatalMsg:
      fprintf(stderr, "      Fatal: [%s:%u, %s] %s\n", context.file, context.line, context.function, localMsg.constData());
      throw QString("Would have aborted here");
    case QtInfoMsg:
      fprintf(stdout, "      Info: [%s:%u, %s] %s\n", context.file, context.line, context.function, localMsg.constData());
  }
}

// TODO move these classes to separate files

class MolsketchErrorPrinter : public CxxTest::ErrorPrinter {
public:
  void enterTest(const CxxTest::TestDescription &description) override {
    CxxTest::OutputStream::endl((*outputStream()) << "Starting test:  " << description.testName());
    ErrorPrinter::enterTest(description);
  }
  void leaveTest(const CxxTest::TestDescription &description) override {
    if (CxxTest::tracker().testSkipped())
      CxxTest::OutputStream::endl((*outputStream()) << "Skipped test:   " << description.testName());
    else if (!CxxTest::tracker().testFailed())
      CxxTest::OutputStream::endl((*outputStream()) << "Completed test: " << description.testName());
    else
      CxxTest::OutputStream::endl((*outputStream()) << "Failed test:    " << description.testName());
    if (!qApp->topLevelWidgets().isEmpty())
      qDebug() << "Warning: Top level widgets left: " << qApp->topLevelWidgets();
  }
  void trace(const char *file, int line, const char *message) override {
    (*outputStream()) << "  ";
    CxxTest::OutputStream::endl((*outputStream()) << "  " << file << ":" << line << ": Trace: " << message);
  }
  void enterSuite(const CxxTest::SuiteDescription &description) override {
    CxxTest::OutputStream::endl((*outputStream()) << "Starting suite:  " << description.suiteName());
    CxxTest::ErrorPrinter::enterSuite(description);
  }
  void leaveSuite(const CxxTest::SuiteDescription &description) override
  {
    CxxTest::OutputStream::endl((*outputStream()) << "Completed suite: " << description.suiteName());
    CxxTest::ErrorPrinter::leaveSuite(description);
  }
  void enterWorld(const CxxTest::WorldDescription &description) override
  {
    CxxTest::OutputStream::endl((*outputStream()) << "Starting world:  " << description.worldName());
    CxxTest::ErrorPrinter::enterWorld(description);
    CxxTest::OutputStream::endl(*outputStream());
  }
  void leaveWorld(const CxxTest::WorldDescription &description) override
  {
    CxxTest::OutputStream::endl((*outputStream()) << "Completed world: " << description.worldName());
    CxxTest::ErrorPrinter::leaveWorld(description);
  }
};

class MolsketchTestListener : public CxxTest::TeeListener {
public:
  CxxTest::XmlPrinter xml_printer;
  MolsketchErrorPrinter error_printer;

  MolsketchTestListener(CXXTEST_STD(ostream) &o = CXXTEST_STD(cout))
    : xml_printer(o)
  {
    setFirst(error_printer);
    setSecond(xml_printer);
  }

  void leaveTest(const CxxTest::TestDescription & testDescription) override {
    AssertionBase::collectGarbage();
    CxxTest::TeeListener::leaveTest(testDescription);
  }

  int run() {
    CxxTest::TestRunner::runAllTests(*this);
    return CxxTest::tracker().failedTests();
  }
};

int main( int argc, char *argv[] ) {
  qInstallMessageHandler(messageOutput);
  char** argdummy = nullptr;
  int argcdummy = 0;
  QApplication app(argcdummy, argdummy);
  QCoreApplication::setOrganizationName("SourceForge");
  QCoreApplication::setOrganizationDomain("sourceforge.net");
  QCoreApplication::setApplicationName("Molsetch");

  std::ofstream ofstr("TEST-cxxtest.xml");
  MolsketchTestListener testPrinter(ofstr);
  CxxTest::RealWorldDescription::_worldName = "Molsketch";
  app.exit(CxxTest::Main< MolsketchTestListener >( testPrinter, argc, argv ));
}

<CxxTest world>
