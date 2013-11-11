# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
TEMPLATE = app
TARGET = ut_cpkeyscreenmodel
QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test
unix { 
    test.commands = /epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe
    autotest.commands = /epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe \
        -xml -o c:/$${TARGET}.xml
}
else:win32 { 
    test.CONFIG += recursive
    autotest.CONFIG += recursive
    build_pass { 
        test.commands = /epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe
        autotest.commands = /epoc32/RELEASE/WINSCW/udeb/$${TARGET}.exe \
            -xml -o c:/$${TARGET}.xml
    }
}
QMAKE_EXTRA_TARGETS += test autotest
HEADERS += ut_cpkeyscreen.h \
    ../src/cpkeyscreenmodel_p.h \
    ../src/cpkeyscreenmodel.h \
    ../src/cpkeyscreenplugin.h \
    ../src/cpkeyscreenview.h \
    ../src/cpkeyscreenconstants.h
    
SOURCES +=  ../src/cpkeyscreenmodel.cpp \
    		../src/cpkeyscreenplugin.cpp \
    		../src/cpkeyscreenview.cpp \
    		ut_cpkeyscreen.cpp
    		
DEPENDPATH += .

LIBS += -lcpframework

symbian { 
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)
    
    LIBS += -lcentralrepository -lfeatmgr
    
    SOURCES += ../src/cpkeyscreenmodel_p.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
}
win32:SOURCES += ../src/cpkeyscreenmodel_win.cpp
symbian:MMP_RULES += SMPSAFE
