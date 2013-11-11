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
HEADERS += ./inc/ut_infowidgetlinehandler.h
SOURCES += ./src/ut_infowidgetlinehandler.cpp

# code to be tested
HEADERS += ../../infowidget/inc/infowidgetlinehandler.h
SOURCES += ../../infowidget/src/infowidgetlinehandler.cpp

# mocks needed for testing
#SOURCES += ../mocks/mock_networkhandlingproxy.cpp


symbian:MMP_RULES += SMPSAFE
