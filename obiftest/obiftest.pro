TEMPLATE = app

HEADERS += $$files(*.h, true)
SOURCES += $$files(*.cpp, true)

QT += widgets
CONFIG += c++17 console
CONFIG -= app_bundle
LIBS += -lmolsketch
# Need to define -L argument for linker to molsketch library
INCLUDEPATH += ../libmolsketch \
    ../obabeliface
