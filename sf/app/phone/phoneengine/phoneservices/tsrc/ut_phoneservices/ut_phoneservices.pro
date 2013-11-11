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
#

TEMPLATE = app
TARGET = 
TARGET.UID3 = 0xEEE39B3F
DEPENDPATH += .
INCLUDEPATH += .
#CONFIG += hb
CONFIG += qtestlib
QT -= gui

symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += ../../../phoneuiutils/inc \
                   ../../../phoneuicontrol/inc \
                   ../../../phoneuiqtviewadapter/src \
                   ../../../../inc
                   
    LIBS += -lxqservice \
            -lxqserviceutil

    TARGET.CAPABILITY = ALL -TCB
    DEFINES += BUILD_PHONESERVICES
}

# Input
HEADERS += ../../inc/phoneservices.h
HEADERS += ../../inc/dialservice.h
HEADERS += ../../inc/dialservicedepricated.h
HEADERS += ../../inc/dtmfservice.h
HEADERS += ../../inc/dtmfservicedepricated.h
HEADERS += ../../inc/urischemehandlerservice.h
SOURCES += ../../src/phoneservices.cpp
SOURCES += unit_tests.cpp


symbian:MMP_RULES += SMPSAFE
