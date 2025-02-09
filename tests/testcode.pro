isEmpty(CXXTEST_PATH) : error("Pass CXXTEST_PATH on command line")
include(findOpenBabel.pri)
include(../settings.pri)
TESTS = $$files(*test.h, true)

HEADERS += $$files($$PWD/../*.h, true)
SOURCES += $$files($$PWD/../tests/*.cpp, true)

DEFINES += MSK_OB_FORMATS_DIR=$$MSK_OB_FORMATS_DIR

CONFIG += c++20 debug

QT += widgets printsupport svg testlib network

INCLUDEPATH += $$CXXTEST_PATH \
    /usr/include/boost/stacktrace \
    ../libmolsketch \
    ../molsketch \
    ../obabeliface

TEMPLATE = lib

TARGET = msktests

cxxtest.output = ${QMAKE_FILE_BASE}.cpp
cxxtest.commands = $$CXXTEST_PATH/bin/cxxtestgen --have-eh --xunit-printer --part ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
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

legacy_dir.target = legacy
legacy_dir.commands = ${MKDIR} legacy
LEGACY_FILE_NAMES += $$files($$PWD/legacy/*, false)
legacy_files.input = LEGACY_FILE_NAMES
legacy_files.depends += legacy
legacy_files.output = legacy/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
legacy_files.variable_out = COPIED_LEGACY_FILES
legacy_files.commands = ${COPY} ${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
legacy_files.CONFIG = target_predeps
QMAKE_EXTRA_COMPILERS += legacy_files

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_CXXFLAGS -= -Wsuggest-override -Werror
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0
LIBS += \
    -L. \
    -lgcov \
    -lboost_stacktrace_addr2line \
    -lmskmain
