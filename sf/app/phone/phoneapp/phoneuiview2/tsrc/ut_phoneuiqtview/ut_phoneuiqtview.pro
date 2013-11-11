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
# Description: Project file for building unit test component
#
#

TEMPLATE = app
TARGET = 
TARGET.UID3 = 0xEA9BEF3F
DEPENDPATH += . ../../src/
INCLUDEPATH += .
CONFIG += hb qtestlib //mobility
//MOBILITY += systeminfo

symbian {

    INCLUDEPATH += /epoc32/include/platform/mw \
                   /epoc32/include/platform/app \
                   /epoc32/include/mw \
                   ../../../phoneuiutils/inc/ \
                   ../../inc/ \
                   ../../../../inc \
                   ./

    LIBS += -lbubblemanager2 \
            -lxqserviceutil \
            -ldialpad \
            -lxqkeycapture

    DEFINES += FT_SYMBIAN_INTEGRATION
} else:win32 {
    INCLUDEPATH += c:/hb/include/hbcore \
        c:/hb/include/hbwidgets \
        ../../inc/ \
        ../../../../inc \
        .

    LIBS += -lbubblemanager2
}

DEFINES += BUILD_PHONEUIQTVIEW

# Input
HEADERS += ../../inc/phoneuiqtview.h \
           ../../inc/phoneuiqtviewif.h \
           ../../inc/phoneaction.h \
           hbtoolbar.h \
           hbvolumesliderpopup.h \
           qsysteminfo.h
SOURCES += ../../src/phoneuiqtview.cpp \
           ../../src/phoneaction.cpp \
           unit_tests.cpp \
           hbtoolbar_stub.cpp \
           hbvolumesliderpopup_stub.cpp \
           qsysteminfo_stub.cpp 
           


symbian:MMP_RULES += SMPSAFE
