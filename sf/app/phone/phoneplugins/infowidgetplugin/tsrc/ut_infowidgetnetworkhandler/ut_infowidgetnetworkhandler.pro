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
DEPENDPATH  += . ../../infowidget/src
INCLUDEPATH += .
INCLUDEPATH += ../common
INCLUDEPATH += ../../inc/
INCLUDEPATH += ../../infowidget/inc/
DEFINES += XQSETTINGSMANAGER_NO_LIBRARY

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib 		\
          -lsymbianmock 	\
          -lcpframework 
          
}
    
# test code
HEADERS += ./inc/ut_infowidgetnetworkhandler.h
SOURCES += ./src/ut_infowidgetnetworkhandler.cpp

# code to be tested
HEADERS += ../../infowidget/inc/infowidgetnetworkhandler.h
SOURCES += ../../infowidget/src/infowidgetnetworkhandler.cpp

# mocks needed for testing
SOURCES += ../mocks/mock_networkhandlingproxy.cpp
SOURCES += ../mocks/mock_cnwsession.cpp

HEADERS += /epoc32/include/platform/mw/xqsettingskey.h \
           /epoc32/include/platform/mw/xqsettingsmanager.h
SOURCES += ../mocks/mock_xqsettingskey.cpp
SOURCES += ../mocks/mock_xqsettingsmanager.cpp


symbian:MMP_RULES += SMPSAFE
