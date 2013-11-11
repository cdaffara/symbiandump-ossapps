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
TARGET = ut_penetworkhandler
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0xEA9BEFAA
DEPENDPATH += . ../../src/
INCLUDEPATH += .
CONFIG += hb qtestlib

symbian {

    INCLUDEPATH += /epoc32/include/platform/mw \
                   /epoc32/include/platform/app \
                   /epoc32/include/mw \
                   ../../../inc/ \
                   ../../../../inc \
                   ../../../../engineinfo/inc \
                   ./

    LIBS += -lQtSystemInfo
    LIBS += -lengineinfo  
}


# Input
HEADERS += ../../inc/penetworkhandler.h 

SOURCES += ../../src/penetworkhandler.cpp \
           ./unit_tests.cpp


symbian:MMP_RULES += SMPSAFE
