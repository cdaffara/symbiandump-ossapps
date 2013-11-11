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
DEPENDPATH  += . ../../infowidget/src ../../infowidget/inc
INCLUDEPATH += .
INCLUDEPATH += ../common
INCLUDEPATH += ../../inc/
INCLUDEPATH += ../../infowidget/inc/
DEFINES += XQSETTINGSMANAGER_NO_LIBRARY
DEFINES += QT_NO_DEBUG_STREAM

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
HEADERS += ./inc/ut_infowidgetengine.h
SOURCES += ./src/ut_infowidgetengine.cpp

# code to be tested
HEADERS += ../../infowidget/inc/infowidgetengine.h
SOURCES += ../../infowidget/src/infowidgetengine.cpp

# mocks needed for testing
HEADERS += ../../infowidget/inc/infowidgetnetworkhandler.h \
           ../../infowidget/inc/infowidgetsathandler.h
HEADERS += /epoc32/include/platform/mw/xqsettingskey.h \
           /epoc32/include/platform/mw/xqsettingsmanager.h

SOURCES += ../mocks/mock_xqsettingsmanager.cpp \
           ../mocks/mock_xqsettingskey.cpp     \
           ../mocks/mock_infowidgetnetworkhandler.cpp \
           ../mocks/mock_infowidgetsathandler.cpp \
           ../mocks/mock_rsatservice.cpp 
           


symbian:MMP_RULES += SMPSAFE
