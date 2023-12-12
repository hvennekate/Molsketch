include(../settings.pri)

TEMPLATE = lib
CONFIG += plugin
HEADERS += $$files(*.h, true)
SOURCES += $$files(*.cpp, true)
FORMS += $$files(*.ui, true)
RESOURCES += $$files(*.qrc, true)
TRANSLATIONS += $$files(*.ts, true)

# CONFIG += lrelease embed_translations
### Defining lrelease and embed_translations explicitly below.
### Rationale: Qt resources have to be unique _by filename_.
###            The default qmake lrelease process will always generate the same output
###            qmake_qmake_qm_files.qrc. Since molsketch itself also has translations,
###            the translations of libmolsketch will be shadowed by these.

### Below code is largely copied from mkspecs/features/lrelease.prf from https://github.com/qt/qtbase/tree/5.15.2
### Changes indicated with "##"

qtPrepareTool(QMAKE_LRELEASE, lrelease)

isEmpty(LRELEASE_DIR): LRELEASE_DIR = .qm
isEmpty(QM_FILES_RESOURCE_PREFIX): QM_FILES_RESOURCE_PREFIX = i18n

lrelease.name = lrelease
lrelease.input = TRANSLATIONS ## do not need EXTRA_TRANSLATIONS
lrelease.output = $$LRELEASE_DIR/${QMAKE_FILE_IN_BASE}.qm
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} $$QMAKE_LRELEASE_FLAGS -qm ${QMAKE_FILE_OUT}
silent: lrelease.commands = @echo lrelease ${QMAKE_FILE_IN} && $$lrelease.commands
lrelease.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += lrelease

for (translation, TRANSLATIONS) { ## again no EXTRA_TRANSLATIONS
    # mirrors $$LRELEASE_DIR/${QMAKE_FILE_IN_BASE}.qm above
    translation = $$basename(translation)
    QM_FILES += $$OUT_PWD/$$LRELEASE_DIR/$$replace(translation, \\..*$, .qm)
}

## embed_translations case only
libmolsketch_qm_files.files = $$QM_FILES
libmolsketch_qm_files.base = $$OUT_PWD/$$LRELEASE_DIR
libmolsketch_qm_files.prefix = $$QM_FILES_RESOURCE_PREFIX
RESOURCES += libmolsketch_qm_files

### end copy from qtbase repo


TARGET = molsketch
DESTDIR = ../lib
QMAKE_CXXFLAGS_DEBUG +=  -g -gstabs
win* : VERSION =

target.path=$${MSK_INSTALL_LIBS}

contains(QT_ARCH, ".*64.*") : target.path = $${MSK_INSTALL_LIBS64}
INSTALLS += target includes
includes.files = $$HEADERS
includes.path = $${MSK_INSTALL_INCLUDES}/libmolsketch
