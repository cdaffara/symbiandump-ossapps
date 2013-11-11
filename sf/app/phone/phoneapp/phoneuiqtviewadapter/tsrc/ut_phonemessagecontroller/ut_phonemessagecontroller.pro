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
TARGET.UID3 = 0xEDA69234
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

LIBS += -lmocklib -lsymbianmock -lphoneuiutils

TARGET.CAPABILITY = ALL -TCB
DEFINES += FT_SYMBIAN_INTEGRATION
DEFINES += BUILD_PHONEUIQTVIEWADAPTER

# Input
HEADERS += ../../inc/phonemessagecontroller.h
SOURCES += ../../src/phonemessagecontroller.cpp
SOURCES += unit_tests.cpp

# MOCKED DEPENDENCIES
HEADERS += ../../inc/phoneapplauncher.h
SOURCES += ../mocks/mock_phoneapplauncher.cpp
SOURCES += ../../../../internal/mocks/phoneapp/phoneuiutils/mock_cphonecenrepproxy.cpp

include(../../../../tsrc/common/phonetest.pri)

symbian:MMP_RULES += SMPSAFE
