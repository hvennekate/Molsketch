include(findOpenBabel.pri)
include(../settings.pri)

TEMPLATE = lib
CONFIG += plugin
HEADERS += obabeliface.h
SOURCES += obabeliface.cpp
INCLUDEPATH += $$MSK_SOURCE_BASE/libmolsketch
LIBS += -L../lib -lmolsketch
CONFIG -= static
CONFIG += shared
QMAKE_LFLAGS -= -static
CONFIG += DLLDESTDIR
TARGET = obabeliface
DESTDIR = ../lib
win* : VERSION =

target.path=$${MSK_INSTALL_LIBS}
INSTALLS += target
