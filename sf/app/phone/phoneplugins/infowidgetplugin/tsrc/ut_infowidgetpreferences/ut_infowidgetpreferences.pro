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
HEADERS += ./inc/ut_infowidgetpreferences.h
SOURCES += ./src/ut_infowidgetpreferences.cpp

# code to be tested
HEADERS += ../../infowidget/inc/infowidgetpreferences.h
SOURCES += ../../infowidget/src/infowidgetpreferences.cpp


symbian:MMP_RULES += SMPSAFE
