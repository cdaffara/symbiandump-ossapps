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
DEPENDPATH += . ../../src/
INCLUDEPATH += .
INCLUDEPATH +=../../../inc 
INCLUDEPATH +=../../../cptelephonyutils/inc
INCLUDEPATH += ../../../tsrc/common
DEFINES += BUILD_TELEPHONYUTILS

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
    INCLUDEPATH += /epoc32/include/mw/QtTest \
        /epoc32/include/platform/mw \
        /epoc32/include/mw/qt
  LIBS += -lmocklib \
        -lsymbianmock \
        -lcpframework
}
    
# test code
HEADERS += ut_cpnetworkplugin.h
SOURCES += ut_cpnetworkplugin.cpp

# code to be tested
HEADERS += ../../src/cpnetworkplugin.h \
           ../../src/cpnetworkpluginview.h \
           ../../src/cpnetworkstatus.h \
           ../../../cptelephonyutils/inc/cpphonelocalisation.h           
       
SOURCES += ../../src/cpnetworkplugin.cpp 

# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_cpnetworkpluginview.cpp
SOURCES += ../../../tsrc/mocks/mock_cpphonelocalisation.cpp
SOURCES += ../../../tsrc/mocks/mock_hbicon.cpp
SOURCES += ../../../tsrc/mocks/mock_cpsettingformentryitemdata.cpp
SOURCES += ../../../tsrc/mocks/mock_cpbasesettingview.cpp
SOURCES += ../../../tsrc/mocks/mock_hbview.cpp
SOURCES += ../../../tsrc/mocks/mock_cpnetworkstatus.cpp



symbian:MMP_RULES += SMPSAFE
