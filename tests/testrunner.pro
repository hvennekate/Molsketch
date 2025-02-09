isEmpty(CXXTEST_PATH) : error("Pass CXXTEST_PATH on command line")
include(findOpenBabel.pri)
CONFIG += c++14
QT += widgets
INCLUDEPATH += $$CXXTEST_PATH \
    /usr/include/boost/stacktrace \

TEMPLATE = app

TARGET = msktestrunner

CXXRUNNER_TEMPLATE = $$PWD/runnerTemplate.tpl
cxxrunner.output = cxxrunner.cpp
cxxrunner.commands = $$CXXTEST_PATH/bin/cxxtestgen --have-eh --xunit-printer --root -o ${QMAKE_FILE_OUT} --template ${QMAKE_FILE_NAME}
cxxrunner.dependency_type = TYPE_C
cxxrunner.input = CXXRUNNER_TEMPLATE
cxxrunner.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += cxxrunner

LIBS += \
    -L. \
    -lgcov \
    -lboost_stacktrace_addr2line \
    -lmskmain \
    -lmsktests \
