include(../obabeliface/findOpenBabel.pri)
include(../settings.pri)
TESTS = $$files(*test.h, true)

SOURCES += $$files($$PWD/../*.cpp, true)
SOURCES -= $$PWD/../molsketch/main.cpp
HEADERS += $$files($$PWD/../*.h, true)
HEADERS -= ${TESTS}
FORMS += $$files($$PWD/../*.ui, true)
RESOURCES += $$files($$PWD/../*.qrc, true)

CONFIG += c++14
QT += widgets printsupport svg testlib network xmlpatterns

INCLUDEPATH += \
    /usr/include/boost/stacktrace \
    ../libmolsketch \
    ../molsketch \
    ../obabeliface

TEMPLATE = lib

TARGET = mskmain

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0
LIBS += -lgcov -lboost_stacktrace_addr2line
