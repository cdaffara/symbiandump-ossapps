#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
TEMPLATE = app
TARGET = 
DEPENDPATH += . ../../src/
INCLUDEPATH += .
INCLUDEPATH +=../../../inc 
INCLUDEPATH +=../../../cptelephonyutils/inc
INCLUDEPATH += ../../../tsrc/common
DEFINES += BUILD_NETWORKPLUGIN BUILD_TELEPHONYUTILS BUILD_PSETWRAPPER \
           QT_BUILD_SYSINFO_LIB QT_MAKEDLL
MOBILITY += systeminfo

QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH +=  $$APP_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib \
        -lsymbianmock \
        -lcpframework
}
    
# test code
HEADERS += ut_cpnetworkstatus.h
SOURCES += ut_cpnetworkstatus.cpp

# code to be tested
HEADERS += ../../src/cpnetworkstatus.h \
           ../../../cptelephonyutils/inc/cpplugincommon.h \
           /epoc32/include/mw/qsysteminfo.h
       
SOURCES += ../../src/cpnetworkstatus.cpp 

# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_cpplugincommon.cpp \
           ../../../tsrc/mocks/mock_hbicon.cpp \
           ../../../tsrc/mocks/mock_cpsettingformentryitemdata.cpp \
           ../../../tsrc/mocks/mock_qsysteminfo.cpp

symbian:MMP_RULES += SMPSAFE
