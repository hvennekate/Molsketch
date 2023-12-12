include(../settings.pri)

TEMPLATE = app
HEADERS += $$files(*.h, true)
SOURCES += $$files(*.cpp, true)
FORMS += $$files(*.ui, true)
RESOURCES += $$files(*.qrc, true)
TRANSLATIONS += $$files(*.ts, true)
RC_ICONS = images/molsketch.ico

CONFIG += lrelease embed_translations
LIBS += -L../lib -lmolsketch
TARGET = molsketch
DESTDIR = ../bin
QMAKE_RPATHDIR = $${MSK_INSTALL_LIBS}
target.path=$${MSK_INSTALL_BINS}
INSTALLS += target
QT += network

unix {
  INSTALLS += icon pixmap mimeicon mimetype desktop appdata

  icon.files = images/molsketch.svg
  icon.path = $$MSK_PREFIX/share/icons/hicolor/scalable/apps

  pixmap.files = images/molsketch.xpm
  pixmap.path = $$MSK_PREFIX/share/pixmaps

  mimeicon.files = images/application-x-molsketch.svg \
                   images/application-x-molsketch-molecule.svg
  mimeicon.path = $$MSK_PREFIX/share/icons/hicolor/scalable/mimetypes

  mimetype.files = molsketch.xml
  mimetype.path = $$MSK_PREFIX/share/mime/packages

  desktop.files = molsketch.desktop
  desktop.path = $$MSK_PREFIX/share/applications

  appdata.files = net.sourceforge.molsketch.appdata.xml
  appdata.path = $$MSK_PREFIX/share/metainfo
}

INCLUDEPATH += \
        $$MSK_SOURCE_BASE/libmolsketch \
        $$MSK_SOURCE_BASE/obabeliface
