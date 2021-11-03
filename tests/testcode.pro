isEmpty(CXXTEST_PATH) : error("Pass CXXTEST_PATH on command line")
include(../obabeliface/findOpenBabel.pri)
include(../settings.pri)
TESTS = $$files(*test.h, true)

HEADERS += $$files($$PWD/../*.h, true)

CONFIG += c++14
QT += widgets printsupport svg testlib network xmlpatterns

INCLUDEPATH += $$CXXTEST_PATH \
    /usr/include/boost/stacktrace \
    ../libmolsketch \
    ../molsketch \
    ../obabeliface

TEMPLATE = lib

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

OTHER_FILES += \
    runnerTemplate.tpl \
    legacy/Carbon-0.2.msk \
    legacy/Carbon-0.2.svg

legacy_files.commands = ${COPY_DIR} $$_PRO_FILE_PWD_/legacy ./legacy
QMAKE_EXTRA_TARGETS += legacy_files
PRE_TARGETDEPS += legacy_files

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0
LIBS += -L. -lgcov -lboost_stacktrace_addr2line -lmskmain
