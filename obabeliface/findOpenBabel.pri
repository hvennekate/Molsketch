defineTest(findOpenBabel) {
        message("Trying to find OpenBabel-2.0")
        possibleOBIncDirs = \
                $${OPENBABEL2_INCLUDE_DIR} \
                C:/openbabel/include \
                /usr/local/include \
                /usr/local/include/openbabel-2.0 \
                /usr/include \
                /usr/include/openbabel-2.0 \
                ${GNUWIN32_DIR}/include
        possibleOBLibDirs = \
                $${OPENBABEL2_LIBRARIES} \
                C:/openbabel/lib \
                /usr/lib \
                /usr/lib64 \
                /usr/local/lib \
                /usr/local/lib64 \
                ${GNUWIN32_DIR}/lib

        for(dir, possibleOBIncDirs) {
                exists($$dir/openbabel/obconversion.h) {
                        OBINCLUDEPATH = $$dir
                        break()
                }
        }
        for(dir, possibleOBLibDirs) {
                exists($${dir}/*openbabel*) {
                        OBLIBS = $${dir}
                        break()
                }
        }
        isEmpty(OBINCLUDEPATH) : error("Could not find OpenBabel-2.0 includes (try setting OPENBABEL2_INCLUDE_DIR)")
        isEmpty(OBLIBS) : error("Could not find OpenBabel-2.0 libs (try setting OPENBABEL2_LIBRARIES)")
        message("Found OpenBabel-2.0.  Includes: $$OBINCLUDEPATH Libs: $$OBLIBS")
        LIBS += -L$${OBLIBS} -lopenbabel
        INCLUDEPATH += $$OBINCLUDEPATH
        export(LIBS)
        export(INCLUDEPATH)
        return(true)
}

unix {
        CONFIG += link_pkgconfig
        packagesExist(openbabel-2.0) {
                message("Using pkgconfig to find OpenBabel.")
                PKGCONFIG += openbabel-2.0
        } else {
                findOpenBabel()
        }
} else {
        findOpenBabel()
}
