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
TARGET = ut_phoneuikeyeventadapter
TARGET.UID3 = 0xEA9BEF3E
DEPENDPATH += . ../../src/
INCLUDEPATH += .
CONFIG += hb qtestlib

symbian {

    INCLUDEPATH += /epoc32/include/platform/mw \
                   /epoc32/include/platform/app \
                   /epoc32/include/mw \
                   ../../../phoneuicontrol/inc/ \
                   ../../../phoneuiutils/inc/ \
                   ../../inc/ \
                   ../../../../inc \
                   ./

}


# Input
HEADERS += ../../inc/phoneuikeyeventadapter.h \
           cphoneuicontroller_stub.h
SOURCES += ../../src/phoneuikeyeventadapter.cpp \
           unit_tests.cpp \
           cphoneuicontroller_stub.cpp
           


symbian:MMP_RULES += SMPSAFE
