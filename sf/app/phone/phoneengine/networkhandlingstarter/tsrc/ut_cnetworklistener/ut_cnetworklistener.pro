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

CONFIG += qtestlib hb mobility
MOBILITY += systeminfo
TEMPLATE = app
TARGET = 

INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../common
DEFINES += QT_NO_DEBUG_STREAM 
DEFINES += BUILD_NETWORKHANDLINGSTARTER QT_BUILD_SYSINFO_LIB QT_MAKEDLL

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib \
          -lsymbianmock \
          -letel \
          -letelmm \
          -lcustomapi
}

# test code
HEADERS += ./inc/ut_cnetworklistener.h
SOURCES += ./src/ut_cnetworklistener.cpp

# code to be tested
HEADERS += ../../inc/cnetworklistener.h
SOURCES += ../../src/cnetworklistener.cpp

# mocks needed for testing
HEADERS += /epoc32/include/mw/qsysteminfo.h
SOURCES += ../mocks/mock_cnwsession.cpp
SOURCES += ../mocks/mock_centralrepository.cpp
SOURCES += ../mocks/mock_qsysteminfo.cpp
SOURCES += ../mocks/mock_psetcontainer.cpp
SOURCES += ../mocks/mock_cpsetrefreshhandler.cpp
HEADERS += ../mocks/mock_cpsetrefreshhandler.h
SOURCES += ../mocks/mock_psetnetwork.cpp

symbian:MMP_RULES += SMPSAFE
