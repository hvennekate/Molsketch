include(findOpenBabel.pri)
include(../settings.pri)

TEMPLATE = lib
CONFIG += plugin
HEADERS += obabeliface.h
SOURCES += obabeliface.cpp
INCLUDEPATH += $$MSK_SOURCE_BASE/libmolsketch
LIBS += -L../lib -lmolsketch$$qtVersionSuffix
CONFIG -= static
CONFIG += shared
QMAKE_LFLAGS -= -static
CONFIG += DLLDESTDIR
TARGET = obabeliface$$qtVersionSuffix
DESTDIR = ../lib
win* : VERSION =

target.path=$${MSK_INSTALL_LIBS}
INSTALLS += target
