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
TARGET.UID3 = 0xEACF41FE
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qtestlib

INCLUDEPATH += /epoc32/include/platform/mw \
               /epoc32/include/platform \
               /epoc32/include/mw \
               ../../../phoneuiutils/inc/ \
               ../../../phoneuiview2/inc \
               ../../../../inc \
               ../../../../tsrc/common

LIBS += -lphoneuiutils

TARGET.CAPABILITY = ALL -TCB

# Input
HEADERS += ../../inc/phonebubblewrapper.h
SOURCES += ../../src/phonebubblewrapper.cpp
SOURCES += unit_tests.cpp

include(../../../../tsrc/common/phonetest.pri)

symbian:MMP_RULES += SMPSAFE
