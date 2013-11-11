#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

TEMPLATE = app

CONFIG(debug, debug|release) {
    LIBS += -L./../../../../../bin/debug -lhsdomainmodel
    DESTDIR = ./debug
} else {
    LIBS += -L./../../../../../bin/release -lhsdomainmodel
    DESTDIR = ./release
}

CONFIG += debug_and_release \
          console

CONFIG += hb

QT += testlib \
      xml \
      sql

coverage:DEFINES += COVERAGE_MEASUREMENT

DEFINES += HOMESCREEN_TEST 

DEPENDPATH += .\
              ./src \
              ../src \
              ../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../inc \
               ../../../hsdomainmodel/inc 
               



symbian: {
    CONFIG += symbian_test
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    TARGET.UID3 = 0x20022F42
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    
    include(installs_symbian.pri)
    
    MMP_RULES += SMPSAFE
}

include(t_hswidgetuninstaller.pri)

win32:include(installs_win32.pri)

