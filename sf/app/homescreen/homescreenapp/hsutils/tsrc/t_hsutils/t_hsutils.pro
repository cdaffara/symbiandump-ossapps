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
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

CONFIG += debug_and_release \
          console

CONFIG += hb mobility
MOBILITY += systeminfo

QT += testlib \
      xml \
      sql

coverage:DEFINES += COVERAGE_MEASUREMENT
bgimage:DEFINES += BG_ENABLED

DEFINES += HOMESCREEN_TEST \
           HSUTILS_TEST

DEPENDPATH += .\
              ./src \
              ../src \
              ../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../inc \
               ../../../serviceproviders/hsmenuserviceprovider/inc

tests_applib: {
    include(../../../common.pri)
    DEFINES += ONLY_MENU_TESTCASES
}

symbian: {
    LIBS+=-lxqservice -lxqserviceutil
    CONFIG += symbian_test
    tests_applib: {
        coverage: CONFIG -= symbian_test
    }
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    TARGET.UID3 = 0x20022F4F
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    
    MMP_RULES += SMPSAFE
}

include(t_hsutils.pri)

win32:include(installs_win32.pri)

