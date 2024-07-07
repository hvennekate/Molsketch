include(variables.pri)
MSK_SOURCE_BASE = $$PWD
QT += widgets printsupport svg
CONFIG += silent c++17
lessThan(QT_MAJOR_VERSION,5): QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wsuggest-override -Werror

buildVars = $$cat(buildvariables)
for(line, $$list($$split(buildVars, $$escape_expand(\\n)))) {
  name = $$getVarName(line)
  !exists($$OUT_PWD/../buildvars/$$name) {
    $$name = $$getVarDefault(line)
    message("Could not find file with value of $$name, assuming default value: $$eval($$name)")
  }
  $$name = $$cat($$OUT_PWD/../buildvars/$$name)
# make variables also available to precompiler
  !equals($$name, false) : DEFINES += "$${name}=\"\\\"$$eval($$name)\\\"\""
}

VERSION = $$cat(version)
CONFIG(static) : DEFINES += QT_STATIC_BUILD
contains(QT_ARCH, ".*64.*") : MSK_INSTALL_LIBS = $${MSK_INSTALL_LIBS64}
OBABEL_IFACE_NAME = libobabeliface.so
win* : OBABEL_IFACE_NAME = obabeliface.dll
isEmpty(RELATIVE_OBABELIFACE) : DEFINES += "OBABEL_IFACE_LOCATION=\"\\\"$$MSK_INSTALL_LIBS/$$OBABEL_IFACE_NAME\\\"\""
!isEmpty(RELATIVE_OBABELIFACE) : DEFINES += "OBABEL_IFACE_LOCATION=\"\\\"obabeliface\\\"\""

