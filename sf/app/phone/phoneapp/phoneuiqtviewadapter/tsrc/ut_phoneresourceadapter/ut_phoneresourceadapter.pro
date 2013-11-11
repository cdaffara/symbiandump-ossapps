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
TARGET.UID3 = 0xEDA69235
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
CONFIG += qtestlib

INCLUDEPATH += \epoc32\include\platform\mw \
               \epoc32\include\platform \
               \epoc32\include\platform\app \
               ../../../phoneuiutils/inc/ \
               ../../../phoneuiview2/inc \
               ../../../../inc \
               ../../../phoneui2/srcdata \
               ../../../../tsrc/common

LIBS += -lphoneuiutils \
         -lmocklib \
        -lsymbianmock

TARGET.CAPABILITY = ALL -TCB
DEFINES += FT_SYMBIAN_INTEGRATION
DEFINES += BUILD_PHONEUIQTVIEWADAPTER

# Input
HEADERS += ../../inc/phoneresourceadapter.h
SOURCES += ../../src/phoneresourceadapter.cpp
HEADERS += ../../inc/phoneuiqtbuttonscontroller.h
SOURCES += ../../src/phoneuiqtbuttonscontroller.cpp
SOURCES += ../../../../tsrc/mocks/orbit/mock_hbicon.cpp
SOURCES += ../../../../tsrc/mocks/phoneuiview2/mock_phoneaction.cpp
SOURCES += ../../../../tsrc/mocks/orbit/mock_hbaction.cpp
SOURCES += unit_tests.cpp

include(../../../../tsrc/common/phonetest.pri)
symbian:MMP_RULES += SMPSAFE
