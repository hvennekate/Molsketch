include(findOpenBabel.pri)
include(../settings.pri)
TESTS = $$files(*test.h, true)

SOURCES += \
    $$files($$PWD/../libmolsketch/*.cpp, true) \
    $$files($$PWD/../molsketch/*.cpp, true) \
    $$files($$PWD/../obabeliface/*.cpp, true)
SOURCES -= $$PWD/../molsketch/main.cpp

HEADERS += $$files($$PWD/../*.h, true)
HEADERS -= $$files($$PWD/../tests/*.h, true) $$files($$PWD/../obiftest/*.h, true)
FORMS += $$files($$PWD/../*.ui, true)
RESOURCES += $$files($$PWD/../*.qrc, true)

CONFIG += c++14 debug
QT += widgets printsupport svg testlib network

INCLUDEPATH += \
    /usr/include/boost/stacktrace \
    ../libmolsketch \
    ../molsketch \
    ../obabeliface \
    ../libmskcore

TEMPLATE = lib

TARGET = mskmain

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0
LIBS += -lgcov -lboost_stacktrace_addr2line
