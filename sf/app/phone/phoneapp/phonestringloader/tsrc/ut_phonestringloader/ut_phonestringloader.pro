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
TARGET.UID3 = 0xEF78FF4E
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
CONFIG += qtestlib

symbian {

    INCLUDEPATH += \epoc32\include\platform\mw \
                   \epoc32\include\platform \
                   ../../../phoneuiview2/inc \
                   ../../../phoneuiutils/inc \
                   ../../../../inc \
                   ../../../phoneui2/srcdata

    LIBS += -lphoneuiqtview
    LIBS += -lbafl

    TARGET.CAPABILITY = ALL -TCB
}

# Input
HEADERS += phoneresourceadapter.h
SOURCES += ../../src/stringloader.cpp
SOURCES += unit_tests.cpp


symbian:MMP_RULES += SMPSAFE
