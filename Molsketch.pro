message("======= Welcome to Molsketch build =======")
REQUIRED_QT_VERSION=5
lessThan(QT_MAJOR_VERSION, $$REQUIRED_QT_VERSION) : error("Molsketch requires at least Qt version $${REQUIRED_QT_VERSION}.")
include(variables.pri)
############## SETTINGS

message("Build settings:")
buildVars = $$cat(buildvariables)
for(buildVar, $$list($$split(buildVars, \\n))) {
  variableName = $$getVarName(buildVar)
  isEmpty($$variableName) {
    $$variableName = $$getVarDefault(buildVar)
    contains(variableName, ".*INSTALL.*") : $$variableName = $$MSK_PREFIX$$eval($$variableName)
  }
  message("$$getVarDescription(buildVar): $${variableName} = $$eval($$variableName)")
  write_file($$OUT_PWD/buildvars/$$variableName, $$variableName)
}

############## setting up sub projects

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
        libmolsketch \
        molsketch
equals(MSK_OBABELIFACE, true) {
    SUBDIRS += obabeliface
    obabeliface.depends = libmolsketch
}

OTHER_FILES += \
    version \
    versionnick \
    CHANGELOG \
    buildvariables \
    COPYING
