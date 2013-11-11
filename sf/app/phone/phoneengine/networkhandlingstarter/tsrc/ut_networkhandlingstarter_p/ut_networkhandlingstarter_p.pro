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
DEFINES += XQ_BUILD_XQSERVICE_LIB

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib \
          -lsymbianmock \
          -lxqserviceutil
}

# test code
HEADERS += ./inc/ut_networkhandlingstarter_p.h
SOURCES += ./src/ut_networkhandlingstarter_p.cpp

# code to be tested
HEADERS += ../../inc/networkhandlingstarter_p.h
SOURCES += ../../src/networkhandlingstarter_p.cpp

# mocks needed for testing
SOURCES += ../mocks/mock_cnetworklistener.cpp
SOURCES += ../mocks/mock_hbdevicemessagebox.cpp
SOURCES += ../mocks/mock_hbaction.cpp
SOURCES += ../mocks/mock_hbinstance.cpp
SOURCES += ../mocks/mock_hbmainwindow.cpp
SOURCES += ../mocks/mock_qgraphicsview.cpp
SOURCES += ../mocks/mock_qabstractscrollarea.cpp
SOURCES += ../mocks/mock_qframe.cpp
HEADERS += /epoc32/include/mw/xqappmgr.h
SOURCES += ../mocks/mock_xqappmgr.cpp
HEADERS += /epoc32/include/mw/xqaiwrequest.h
SOURCES += ../mocks/mock_xqaiwrequest.cpp
SOURCES += ../mocks/mock_hbtranslator.cpp



symbian:MMP_RULES += SMPSAFE
