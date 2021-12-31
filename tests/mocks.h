/***************************************************************************
 *   Copyright (C) 2018 by Hendrik Vennekate, Hendrik.Vennekate@posteo.de  *
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

#ifndef MOCKS_H
#define MOCKS_H

/**
 * Use to create mock function with an arbitrary return value and arguments.
 * Will provide a callback ("Callback" appended to the function name) and
 * a default return value ("DefaultReturn" appended to the function name).
 * The default return value will be returned if the callback is undefined.
 *
 * To pass multiple arguments, it is suggested to define a macro COMMA.
 */
#define MOCK(RETURNTYPE, FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<RETURNTYPE(ARGUMENTS)> FUNCTIONNAME##Callback; \
    RETURNTYPE FUNCTIONNAME##DefaultReturn; \
    RETURNTYPE FUNCTIONNAME(ARGUMENTS) override { \
        if(FUNCTIONNAME##Callback) \
            return FUNCTIONNAME##Callback(ARGUMENTNAMES) ; \
        return FUNCTIONNAME##DefaultReturn; \
        }

/**
 * Use to create mock function with an arbitrary return value and arguments
 * but with const qualifier.
 * Will provide a callback ("Callback" appended to the function name) and
 * a default return value ("DefaultReturn" appended to the function name).
 * The default return value will be returned if the callback is undefined.
 *
 * To pass multiple arguments, it is suggested to define a macro COMMA.
 */
#define MOCK_CONST(RETURNTYPE, FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<RETURNTYPE(ARGUMENTS)> FUNCTIONNAME##Callback; \
    RETURNTYPE FUNCTIONNAME##DefaultReturn; \
    RETURNTYPE FUNCTIONNAME(ARGUMENTS) const override { \
        if(FUNCTIONNAME##Callback) \
            return FUNCTIONNAME##Callback(ARGUMENTNAMES) ; \
        return FUNCTIONNAME##DefaultReturn; \
        }


/**
 * Use to create mock function of void type.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed in the function if it has been defined.
 */
#define VOID_MOCK(FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<void(ARGUMENTS)> FUNCTIONNAME##Callback; \
    void FUNCTIONNAME(ARGUMENTS) override { \
        if(FUNCTIONNAME##Callback) \
            FUNCTIONNAME##Callback(ARGUMENTNAMES) ; \
        }

/**
 * Use to create mock function of void type
 * but with const qualifier.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed in the function if it has been defined.
 */
#define VOID_MOCK_CONST(FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<void(ARGUMENTS)> FUNCTIONNAME##Callback; \
    void FUNCTIONNAME(ARGUMENTS) const override { \
        if(FUNCTIONNAME##Callback) \
            FUNCTIONNAME##Callback(ARGUMENTNAMES) ; \
        }

/**
 * Use to create a mock function of arbitrary return type.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed if it has been defined.
 * Otherwise, the parent implementation will be called.
 * Requires `super` to be defined as the parent class.
 */
#define SUPER_MOCK(RETURNTYPE, FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<RETURNTYPE(ARGUMENTS)> FUNCTIONNAME##Callback; \
    RETURNTYPE FUNCTIONNAME(ARGUMENTS) override { \
        if(FUNCTIONNAME##Callback) \
            return FUNCTIONNAME##Callback(ARGUMENTNAMES) ;\
        return super::FUNCTIONNAME(ARGUMENTNAMES); \
    }

/**
 * Use to create a mock function of arbitrary return type
 * but with const qualifier.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed if it has been defined.
 * Otherwise, the parent implementation will be called.
 * Requires `super` to be defined as the parent class.
 */
#define SUPER_MOCK_CONST(RETURNTYPE, FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<RETURNTYPE(ARGUMENTS)> FUNCTIONNAME##Callback; \
    RETURNTYPE FUNCTIONNAME(ARGUMENTS) const override { \
        if(FUNCTIONNAME##Callback) \
            return FUNCTIONNAME##Callback(ARGUMENTNAMES) ;\
        return super::FUNCTIONNAME(ARGUMENTNAMES); \
    }


/**
 * Use to create a mock function of void return type.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed if it has been defined.
 * Otherwise, the parent implementation will be called.
 * Requires `super` to be defined as the parent class.
 */
#define VOID_SUPER_MOCK(FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<void(ARGUMENTS)> FUNCTIONNAME##Callback; \
    void FUNCTIONNAME(ARGUMENTS) override { \
        if(FUNCTIONNAME##Callback) \
            FUNCTIONNAME##Callback(ARGUMENTNAMES) ;\
        else \
            super::FUNCTIONNAME(ARGUMENTNAMES); \
    }

/**
 * Use to create a mock function of arbitrary return type.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed if it has been defined.
 * Otherwise, the parent implementation will be called and a
 * failing assertion will be executed.
 * Requires `super` to be defined as the parent class.
 */
#define REJECT_MOCK(RETURNTYPE, FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<RETURNTYPE(ARGUMENTS)> FUNCTIONNAME##Callback; \
    RETURNTYPE FUNCTIONNAME(ARGUMENTS) override { \
        if(FUNCTIONNAME##Callback) \
            return FUNCTIONNAME##Callback(ARGUMENTNAMES) ; \
        Mocks::reject(#FUNCTIONNAME " should not be called"); \
        return super::FUNCTIONNAME(ARGUMENTNAMES); \
    }

/**
 * Use to create a mock function of void return type.
 * Creates a callback ("Callback" appended to the function name)
 * which will be executed if it has been defined.
 * Otherwise, a failing assertion will be executed.
 * Afterwards, the parent implementation will be called.
 * Requires `super` to be defined as the parent class.
 */
#define VOID_REJECT_MOCK(FUNCTIONNAME, ARGUMENTS, ARGUMENTNAMES) \
    std::function<void(ARGUMENTS)> FUNCTIONNAME##Callback; \
    void FUNCTIONNAME(ARGUMENTS) override { \
  qDebug() << "calling function"; \
        if(FUNCTIONNAME##Callback) FUNCTIONNAME##Callback(ARGUMENTNAMES) ; \
        else Mocks::reject(#FUNCTIONNAME " should not be called"); \
        super::FUNCTIONNAME(ARGUMENTNAMES); \
    }

namespace Mocks {
  void reject(const char *message);
}


#define COMMA ,

#endif // MOCKS_H
