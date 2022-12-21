/***************************************************************************
 *   Copyright (C) 2022 by Hendrik Vennekate                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <functional>
#include <QApplication>
#include <QDebug>
#include <QLibrary>
#include <QCommandLineParser>
#include "obabeliface.h"
#include "molecule.h"

using namespace Molsketch;

template<class FUNCTION_POINTER_TYPE, class... ARGS>
void invoke(QLibrary &lib, const char *functionName, ARGS... args) {
    auto functionToInvoke = (FUNCTION_POINTER_TYPE) lib.resolve(functionName);
    if (functionToInvoke) {
        qDebug() << "Testing:" << functionName;
        qDebug() << "Invocation result:" << functionToInvoke(args...);
    } else {
        qDebug() << "Failed to find:" << functionName;
        exit(2);
    }
}

//template<typename FUNCTION_POINTER>
//void checkFunctionInvocation(QLibrary &lib, const char *functionName) {
//    FUNCTION_POINTER outputFormats = (FUNCTION_POINTER) lib.resolve(functionName);
//    if (outputFormats) {
//        qDebug() << "Testing:" << functionName;
//        qDebug() << "Function invocation result:" << outputFormats();
//    } else {
//        qDebug() << "Failed to find:" << functionName;
//        exit(2);
//    }
//}

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("OpenBabel Interface Tester");
    parser.addHelpOption();
    parser.addPositionalArgument("lib", "Shared library file to test");
    parser.process(app);
    auto args = parser.positionalArguments();
    if (args.size() != 1) {
        qInfo() << "Need exactly one argument; use '-h' option for help.";
        return 0;
    }

    auto libFile = args.at(0);
    QLibrary lib(libFile);
    qDebug() << "Loading library from file:" << libFile;
    lib.load();
    if (!lib.isLoaded()) {
        qDebug() << "Error loading library:" << lib.errorString();
        return 1;
    }

    qDebug() << "Starting test invocation of OpenBabel interface methods";

    invoke<formatsFunctionPointer>(lib, OUTPUT_FORMATS);
    invoke<formatsFunctionPointer>(lib, INPUT_FORMATS);
    invoke<formatAvailablePointer>(lib, INCHI_AVAILABLE);
    invoke<formatAvailablePointer>(lib, GEN2D_AVAILABLE);

    auto atomA = new Atom(QPointF(0,0), "C");
    auto atomB = new Atom(QPointF(100,0), "C");
    auto bond = new Bond(atomA, atomB);
    auto molecule = new Molecule({atomA, atomB}, {bond});

    invoke<fromInChIFunctionPointer>(lib, FROM_INCHI, "1S/C2H6/c1-2/h1-2H3");

    invoke<formatAvailablePointer>(lib, INCHI_AVAILABLE);
    invoke<formatAvailablePointer>(lib, GEN2D_AVAILABLE);

    invoke<optimizeCoordsPointer>(lib, OPTIMIZE_COORDS, molecule);
//    invoke<loadFileFunctionPointer>(lib, LOAD_FILE, "testfile");  // TODO
//    invoke<saveFileFunctionPointer>(lib, SAVE_FILE, "testoutfile", molecule); // TODO
//    QVector<unsigned int> symmetryClasses;
//    invoke<getSymmetryClassesFunctionPointer>(lib, GET_SYMMETRY_CLASSES, molecule, symmetryClasses); // TODO unused
    invoke<chiralAtomsFunctionPointer>(lib, CHIRAL_ATOMS, molecule); // TODO unused
//    invoke<callOsraFunctionPointer>(lib, CALL_OSRA, "testosrafile"); // TODO

    delete molecule;
    qDebug() << "Completed test invocations.";

    return 0;
}
