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
# Description: Project file for building unit test component
#
#

TEMPLATE = app
TARGET.UID3 = 0xEDA69435
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
CONFIG += qtestlib

INCLUDEPATH += /epoc32/include/platform/mw \
               /epoc32/include/platform \
               /epoc32/include/platform/app \
               ../../../phoneuiutils/inc/ \
               ../../../phoneuiview2/inc \
               ../../../../inc \
               ../../../../phoneengine/inc \
               ../../../phoneui2/srcdata \
               ../../../../tsrc/common \
               ../../../phonemediatorcenter/inc \
               ../../../phoneuicontrol/inc

LIBS += -lphoneuiutils \
        -lmocklib \
        -lsymbianmock \
        -lengineinfo

TARGET.CAPABILITY = ALL -TCB
DEFINES += BUILD_PHONEUIQTVIEWADAPTER

# Input
HEADERS += ../../inc/phonecallheadermanager.h
HEADERS += ../../inc/phonebubblewrapper.h
HEADERS += bubblemanagerif_stub.h
HEADERS += phoneuiqtviewif_stub.h
SOURCES += ../../src/phonecallheadermanager.cpp
SOURCES += ../../../../tsrc/mocks/phoneuiqtviewadapter/mock_phonebubblewrapper.cpp
SOURCES += ../../../../tsrc/mocks/phoneuiqtviewadapter/mock_phonecallheaderutil.cpp
SOURCES += ../../../../tsrc/stubs/stubs.cpp
SOURCES += unit_tests.cpp

include(../../../../tsrc/common/phonetest.pri)
