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

TARGET = testvideosettingsaccesspointentry
TEMPLATE = app

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += inc \
               stub/inc \
               ../../inc \
               ../../../../inc \

CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lcpframework \
        -lxqutils.dll

# Input
HEADERS += inc/testvideosettingsaccesspointentry.h \
           ../../inc/videosettingsaccesspointentry.h \
           stub/inc/videosettingsgroup.h \
           stub/inc/cpitemdatahelper.h \
           stub/inc/cpsettingformentryitemdata.h \
           stub/inc/cmapplsettingsui.h \
           stub/inc/cmmanager_shim.h \
           stub/inc/cmconnectionmethod_shim.h \
   
SOURCES += src/testvideosettingsaccesspointentry.cpp \
           ../../src/videosettingsaccesspointentry.cpp \
           stub/src/videosettingsgroup.cpp \
           stub/src/cpitemdatahelper.cpp \
           stub/src/cpsettingformentryitemdata.cpp \
           stub/src/cmapplsettingsui.cpp \
           stub/src/cmmanager_shim.cpp \
           stub/src/cmconnectionmethod_shim.cpp \

TARGET.CAPABILITY = All -Tcb
