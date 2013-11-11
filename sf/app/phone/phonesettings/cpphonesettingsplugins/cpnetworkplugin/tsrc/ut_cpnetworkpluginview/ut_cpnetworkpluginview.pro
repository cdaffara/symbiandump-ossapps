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
INCLUDEPATH += . ../../src/
INCLUDEPATH += ../../../cptelephonyutils/inc


QT -= gui
QT += testlib

symbian: {
  #CONFIG += no_icon
  INCLUDEPATH += /epoc32/include/mw/QtTest \
        /epoc32/include/platform/mw \
        /epoc32/include/mw/qt
  LIBS += -lmocklib \
        -lsymbianmock \
        -lcpframework
}
    
# test code
HEADERS += ut_cpnetworkpluginview.h
SOURCES += ut_cpnetworkpluginview.cpp

# code to be tested
HEADERS += ../../src/cpnetworkpluginview.h 
SOURCES += ../../src/cpnetworkpluginview.cpp 

# mocks needed for testing
HEADERS += ../../src/cpnetworkpluginform.h
SOURCES += ../../../tsrc/mocks/mock_cpnetworkpluginform.cpp
SOURCES += ../../../tsrc/mocks/mock_cpbasesettingview.cpp
SOURCES += ../../../tsrc/mocks/mock_hbview.cpp
   

symbian:MMP_RULES += SMPSAFE
