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
MOBILITY = serviceframework
TEMPLATE = app
TARGET = 
INCLUDEPATH += .
INCLUDEPATH += ../common
INCLUDEPATH += ../../inc/
INCLUDEPATH += ../../infowidget/inc/
DEFINES += QT_NO_DEBUG_STREAM

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib 		\
          -lsymbianmock     \
          -lhswidgetmodel
}

# test code
HEADERS += ./inc/ut_infowidgetplugin.h
SOURCES += ./src/ut_infowidgetplugin.cpp

# code to be tested
HEADERS += ../../inc/infowidgetplugin.h
SOURCES += ../../src/infowidgetplugin.cpp

# mocks needed for testing
HEADERS += ../../infowidget/inc/infowidget.h
SOURCES += ../mocks/mock_infowidget.cpp
SOURCES += ../mocks/mock_qserviceplugininterface.cpp
SOURCES += ../mocks/mock_qserviceinterfacedescriptor.cpp

symbian:MMP_RULES += SMPSAFE
