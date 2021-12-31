CONFIG += silent
CONFIG += c++14

DEFINES += QMAKEBUILD

sourceDir = $$_PRO_FILE_PWD_
message("Sources in: $$sourceDir")

VERSION = $$cat(version)
VERSIONNICK = $$cat(versionnick)


########### Reading version and default paths
message("Installation directories and corresponding qmake variables:")

defaultVars = \
	Global_prefix,MSK_INSTALL_PREFIX,/usr/local,C:/Program_Files/MolsKetch;\
	Executable,MSK_INSTALL_BINS,/bin,;\
	Headers,MSK_INSTALL_INCLUDES,/include/molsketch,/include;\
	Molecule_library,MSK_INSTALL_LIBRARY,/share/molsketch/library,/library;\
	Custom_molecule_library,MSK_INSTALL_CUSTOM,/share/molsketch/library/custom,/library/custom;\
	Documentation,MSK_INSTALL_DOCS,/share/doc/molsketch,/doc;
contains(QT_ARCH, ".*64.*") : defaultVars += Libraries,MSK_INSTALL_LIBS,/lib64,;
else : defaultVars += Libraries,MSK_INSTALL_LIBS,/lib,;

defaultVars += Version,MSK_VERSION,$$cat(version);\
	Nickname,MSK_Version_NICK,$$cat(versionnick)

for(DV, $$list($$split(defaultVars, ;))) {
	variableDescription = $$section(DV, ",", 0, 0)
	variableName = $$section(DV, ",", 1, 1)
	variableDefaultValue = $$section(DV, ",", 2, 2)
	contains(variableName, ".*INSTALL.*"){
		variableDefaultValue = $${MSK_INSTALL_PREFIX}$$section(DV, ",", 2, 2)
		win*: variableDefaultValue = $${MSK_INSTALL_PREFIX}$$section(DV, ",", 3, 3)
	}
	isEmpty($$variableName) : $$variableName = $$replace(variableDefaultValue, _, " ") # TODO not so elegant...
	DEFINES += "$${variableName}=\"\\\"$$eval($$variableName)\\\"\""
	message("$$split(variableDescription, _): $${variableName} = $$eval($$variableName)")
}

VERSION = $${MSK_VERSION}

HEADERS += \
    libmolsketch/src/atom.h \
    libmolsketch/src/atomnumberitem.h \
    libmolsketch/src/bond.h \
    libmolsketch/src/commands.h \
    libmolsketch/src/electronsystem.h \
    libmolsketch/src/element.h \
    libmolsketch/src/fileio.h \
    libmolsketch/src/graphsymitem.h \
    libmolsketch/src/itemplugin.h \
    libmolsketch/src/math2d.h \
    libmolsketch/src/mimemolecule.h \
    libmolsketch/src/minimise.h \
    libmolsketch/src/molecule.h \
    libmolsketch/src/molinputitem.h \
    libmolsketch/src/mollibitem.h \
    libmolsketch/src/molscene.h \
    libmolsketch/src/molview.h \
    libmolsketch/src/ring.h \
    libmolsketch/src/smilesitem.h \
    libmolsketch/src/stereocenteritem.h \
    libmolsketch/src/TextInputItem.h \
    libmolsketch/src/actions/coloraction.h \
    libmolsketch/src/graphicsitem.h \
    libmolsketch/src/actions/linewidthaction.h \
    libmolsketch/src/abstractxmlobject.h \
    libmolsketch/src/actions/genericaction.h \
    libmolsketch/src/actions/multiaction.h \
    libmolsketch/src/actions/incdecaction.h \
    libmolsketch/src/actions/connectaction.h \
    libmolsketch/src/actions/minimizeaction.h \
    libmolsketch/src/actions/rotateaction.h \
    libmolsketch/src/actions/transformcommand.h \
    libmolsketch/src/actions/reactionarrowaction.h \
    libmolsketch/src/arrow.h \
    libmolsketch/src/actions/periodictablewidget.h \
    libmolsketch/src/actions/bondtypewidget.h \
    libmolsketch/src/actions/drawaction.h \
    libmolsketch/src/actions/ringaction.h \
    libmolsketch/src/actions/mechanismarrowaction.h \
    libmolsketch/src/actions/pluginaction.h \
    libmolsketch/src/actions/deleteaction.h \
    libmolsketch/src/actions/bondtypeaction.h \
    libmolsketch/src/actions/itemtypewidget.h \
    libmolsketch/src/actions/itemtypeaction.h \
    libmolsketch/src/actions/arrowtypeaction.h \
    libmolsketch/src/actions/arrowtypewidget.h \
    libmolsketch/src/actions/flipbondaction.h \
    libmolsketch/src/stringify.h \
    libmolsketch/src/frame.h \
    libmolsketch/src/actions/frameaction.h \
    libmolsketch/src/actions/frametypewidget.h \
    libmolsketch/src/actions/frametypeaction.h \
    libmolsketch/src/actions/itemgrouptypeaction.h \
    libmolsketch/src/arrowpopup.h \
    libmolsketch/src/coordinatedelegate.h \
    libmolsketch/src/coordinatemodel.h \
    libmolsketch/src/coordinatetableview.h \
    libmolsketch/src/grid.h \
    libmolsketch/src/actions/flipstereobondsaction.h \
    libmolsketch/src/generics.h \
    libmolsketch/src/actions/changemoleculenameaction.h \
    libmolsketch/src/moleculepopup.h \
    libmolsketch/src/helptextonemptylabel.h \
    libmolsketch/src/helptextemptydock.h \
    libmolsketch/src/actions/abstractrecursiveitemaction.h \
    libmolsketch/src/actions/abstractitemaction.h \
    libmolsketch/src/atompopup.h \
    libmolsketch/src/propertieswidget.h \
    molsketch/src/helpforemptytoolbox.h

SOURCES += \
    libmolsketch/src/atom.cpp \
    libmolsketch/src/atomnumberitem.cpp \
    libmolsketch/src/bond.cpp \
    libmolsketch/src/commands.cpp \
    libmolsketch/src/electronsystem.cpp \
    libmolsketch/src/element.cpp \
    libmolsketch/src/fileio.cpp \
    libmolsketch/src/graphsymitem.cpp \
    libmolsketch/src/itemplugin.cpp \
    libmolsketch/src/mimemolecule.cpp \
    libmolsketch/src/minimise.cpp \
    libmolsketch/src/molecule.cpp \
    libmolsketch/src/molinputitem.cpp \
    libmolsketch/src/mollibitem.cpp \
    libmolsketch/src/molscene.cpp \
    libmolsketch/src/molview.cpp \
    libmolsketch/src/ring.cpp \
    libmolsketch/src/smilesitem.cpp \
    libmolsketch/src/stereocenteritem.cpp \
    libmolsketch/src/TextInputItem.cpp \
    libmolsketch/src/actions/coloraction.cpp \
    libmolsketch/src/graphicsitem.cpp \
    libmolsketch/src/actions/linewidthaction.cpp \
    libmolsketch/src/abstractxmlobject.cpp \
    libmolsketch/src/actions/genericaction.cpp \
    libmolsketch/src/actions/multiaction.cpp \
    libmolsketch/src/actions/incdecaction.cpp \
    libmolsketch/src/actions/connectaction.cpp \
    libmolsketch/src/actions/minimizeaction.cpp \
    libmolsketch/src/actions/rotateaction.cpp \
    libmolsketch/src/actions/transformcommand.cpp \
    libmolsketch/src/actions/reactionarrowaction.cpp \
    libmolsketch/src/arrow.cpp \
    libmolsketch/src/actions/periodictablewidget.cpp \
    libmolsketch/src/actions/bondtypewidget.cpp \
    libmolsketch/src/actions/drawaction.cpp \
    libmolsketch/src/actions/ringaction.cpp \
    libmolsketch/src/actions/mechanismarrowaction.cpp \
    libmolsketch/src/actions/pluginaction.cpp \
    libmolsketch/src/actions/deleteaction.cpp \
    libmolsketch/src/actions/bondtypeaction.cpp \
    libmolsketch/src/actions/itemtypewidget.cpp \
    libmolsketch/src/actions/itemtypeaction.cpp \
    libmolsketch/src/actions/arrowtypeaction.cpp \
    libmolsketch/src/actions/arrowtypewidget.cpp \
    libmolsketch/src/actions/flipbondaction.cpp \
    libmolsketch/src/frame.cpp \
    libmolsketch/src/actions/frameaction.cpp \
    libmolsketch/src/actions/frametypewidget.cpp \
    libmolsketch/src/actions/frametypeaction.cpp \
    libmolsketch/src/actions/itemgrouptypeaction.cpp \
    libmolsketch/src/arrowpopup.cpp \
    libmolsketch/src/coordinatedelegate.cpp \
    libmolsketch/src/coordinatemodel.cpp \
    libmolsketch/src/coordinatetableview.cpp \
    libmolsketch/src/grid.cpp \
    libmolsketch/src/actions/flipstereobondsaction.cpp \
    libmolsketch/src/generics.cpp \
    libmolsketch/src/actions/changemoleculenameaction.cpp \
    libmolsketch/src/moleculepopup.cpp \
    libmolsketch/src/helptextonemptylabel.cpp \
    libmolsketch/src/helptextemptydock.cpp \
    libmolsketch/src/actions/abstractrecursiveitemaction.cpp \
    libmolsketch/src/actions/abstractitemaction.cpp \
    libmolsketch/src/atompopup.cpp \
    libmolsketch/src/propertieswidget.cpp \
    molsketch/src/helpforemptytoolbox.cpp

FORMS += \
    libmolsketch/src/arrowpopup.ui

RESOURCES += \
    libmolsketch/src/tools/toolicons.qrc

QT += svg printsupport

TEMPLATE = app

RESOURCES += \
    molsketch/src/molsketch.qrc \
    molsketch/src/oxygenicons.qrc

FORMS += \
    molsketch/src/drawwidget.ui \
    molsketch/src/settings.ui \
    libmolsketch/src/arrowpopup.ui \
    libmolsketch/src/moleculepopup.ui \
    libmolsketch/src/atompopup.ui

HEADERS += \
    molsketch/src/mainwindow.h \
    molsketch/src/settings.h \
    molsketch/src/librarylistwidget.h

SOURCES += \
    molsketch/src/main.cpp \
    molsketch/src/mainwindow.cpp \
    molsketch/src/settings.cpp \
    molsketch/src/librarylistwidget.cpp

INCLUDEPATH += \
	libmolsketch/src \
	obabeliface

TARGET = Molsketch
DESTDIR = bin

target.path=bin
INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml
OTHER_FILES += \
    version \
    versionnick

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

