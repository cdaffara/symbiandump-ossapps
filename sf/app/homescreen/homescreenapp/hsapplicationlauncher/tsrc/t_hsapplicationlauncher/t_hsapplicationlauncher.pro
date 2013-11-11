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

QT += testlib

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

DEFINES += HOMESCREEN_TEST

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH += .\
              ./src \
              ./inc \
              ../../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc

symbian: {
    CONFIG += symbian_test    
    CONFIG -= symbian_i18n
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    TARGET.UID3 = 0x20022F6E
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    LIBS+= -lssmcmn -lcentralrepository -lcenrepnotifhandler        
    MMP_RULES += SMPSAFE 
}

include(t_hsapplicationlauncher.pri)