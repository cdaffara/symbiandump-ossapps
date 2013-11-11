#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
}
else {
    DESTDIR = ./release
}

CONFIG += debug_and_release \
          console

QT += testlib

DEFINES += HOMESCREEN_TEST

DEPENDPATH += .\
              ./src \
              ./inc \
              ../../src \
              ../../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../inc

symbian: {
    CONFIG += symbian_test
    load(hb.prf)
    CONFIG -= symbian_i18n      
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    TARGET.UID3 = 0x20022F73
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    LIBS += -lxqservice
    LIBS += -lxqserviceutil
    
    MMP_RULES += SMPSAFE
} else {
    error("Only Symbian supported!")
}

include(t_hshomescreenclientplugin.pri)

