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
# Description: tests for About plugin
#

TEMPLATE = app

CONFIG += debug_and_release

QT += testlib

symbian {
     LIBS += -lsysversioninfo \
            -lsysutil \
            -lxqutils 
}

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

symbian: LIBS += -lcpframework 
CONFIG += hb 

INCLUDEPATH += ./inc
                               


DEPENDPATH += ./inc \
              ./src \
              ../inc \
              ../src 

DEFINES += ABOUT_TEST 

symbian {
    TARGET.UID3 = 0x2002BCD5
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
}
symbian:MMP_RULES += SMPSAFE
include(t_aboutplugin.pri)
