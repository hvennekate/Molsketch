isEmpty(CXXTEST_PATH) : error("Pass CXXTEST_PATH on command line")
include(../obabeliface/findOpenBabel.pri)
include(../settings.pri)
# TODO: split off library to allow for faster linking
TESTS = $$files(*test.h, true)

SOURCES += $$files($$PWD/../*.cpp, true)
SOURCES -= $$PWD/../molsketch/main.cpp
HEADERS += $$files($$PWD/../*.h, true)
FORMS += $$files($$PWD/../*.ui, true)
RESOURCES += $$files($$PWD/../*.qrc, true)

CONFIG += c++14
QT += widgets printsupport svg testlib network xmlpatterns

INCLUDEPATH += $$CXXTEST_PATH \
    /usr/include/boost/stacktrace \
    ../libmolsketch \
    ../molsketch \
    ../obabeliface

TEMPLATE = app

TARGET = msktests

cxxtest.output = ${QMAKE_FILE_BASE}.cpp
cxxtest.commands = $$CXXTEST_PATH/bin/cxxtestgen --xunit-printer --part ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
cxxtest.depency_type = TYPE_C
cxxtest.input = TESTS
cxxtest.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += cxxtest

changelogSyntax.target = changelogsuccess
changelogSyntax.commands = xmllint --noout $$PWD/../CHANGELOG && touch $$changelogSyntax.target
changelogSyntax.depends = $$PWD/../CHANGELOG
QMAKE_EXTRA_TARGETS += changelogSyntax
POST_TARGETDEPS += $$changelogSyntax.target

CXXRUNNER_TEMPLATE = $$PWD/runnerTemplate.tpl
cxxrunner.output = cxxrunner.cpp
cxxrunner.commands = $$CXXTEST_PATH/bin/cxxtestgen --have-eh --xunit-printer --root -o ${QMAKE_FILE_OUT} --template ${QMAKE_FILE_NAME}
cxxrunner.dependency_type = TYPE_C
cxxrunner.input = CXXRUNNER_TEMPLATE
cxxrunner.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += cxxrunner

OTHER_FILES += \
    runnerTemplate.tpl \
    legacy/Carbon-0.2.msk \
    legacy/Carbon-0.2.svg

LEGACY_FILE_NAMES += $$files($$PWD/legacy/*, false)
legacy_files.input = LEGACY_FILE_NAMES
legacy_files.output = legacy/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
legacy_files.commands = ${COPY} ${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
legacy_files.CONFIG = target_predeps
QMAKE_EXTRA_COMPILERS += legacy_files

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0
LIBS += -lgcov -lboost_stacktrace_addr2line
