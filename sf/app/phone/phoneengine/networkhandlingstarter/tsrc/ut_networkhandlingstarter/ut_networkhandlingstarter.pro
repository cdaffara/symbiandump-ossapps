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

CONFIG += qtestlib hb
TEMPLATE = app
TARGET = 

INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../common
DEFINES += QT_NO_DEBUG_STREAM BUILD_NETWORKHANDLINGSTARTER

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib \
          -lsymbianmock \
          -lxqservice
}

# test code
HEADERS += ./inc/ut_networkhandlingstarter.h
SOURCES += ./src/ut_networkhandlingstarter.cpp

# code to be tested
HEADERS += ../../inc/networkhandlingstarter.h
SOURCES += ../../src/networkhandlingstarter.cpp

# mocks needed for testing
HEADERS += ../../inc/networkhandlingstarter_p.h
SOURCES += ../mocks/mock_networkhandlingstarter_p.cpp \
           ../mocks/mock_tstasksettings.cpp



symbian:MMP_RULES += SMPSAFE
