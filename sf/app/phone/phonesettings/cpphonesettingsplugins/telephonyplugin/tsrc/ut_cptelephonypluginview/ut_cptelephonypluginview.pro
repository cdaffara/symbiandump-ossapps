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
# Description: 
#
#

CONFIG += qtestlib hb
TEMPLATE = app
TARGET = 
DEPENDPATH += . ../../src/
INCLUDEPATH += .
INCLUDEPATH +=../../../cptelephonyutils/inc
DEFINES += BUILD_CP_FRAMEWORK

QT -= gui
QT += testlib

symbian: {
  #CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  
  LIBS += -lmocklib \
        -lsymbianmock \
        -lcpframework
}

# test code
HEADERS += ut_cptelephonypluginview.h
SOURCES += ut_cptelephonypluginview.cpp

# code to be tested
HEADERS += ../../src/cptelephonypluginview.h
           
SOURCES += ../../src/cptelephonypluginview.cpp 

# mocks needed for testing
HEADERS += /epoc32/include/platform/mw/cpitemdatahelper.h
HEADERS += /epoc32/include/platform/mw/cpsettingformitemdata.h
HEADERS += cpbasesettingview.h

SOURCES += ../../../tsrc/mocks/mock_cppluginloader.cpp
SOURCES += ../../../tsrc/mocks/mock_cpbasesettingview.cpp
SOURCES += ../../../tsrc/mocks/mock_cpitemdatahelper.cpp
SOURCES += ../../../tsrc/mocks/mock_cpsettingformitemdata.cpp

symbian:MMP_RULES += SMPSAFE