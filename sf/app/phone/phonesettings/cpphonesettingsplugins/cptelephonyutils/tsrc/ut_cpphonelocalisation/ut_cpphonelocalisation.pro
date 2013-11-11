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

CONFIG += qtestlib
CONFIG += hb
TEMPLATE = app
TARGET = 
DEPENDPATH += . ../../src/
INCLUDEPATH += . ../../inc/
INCLUDEPATH += ../../../tsrc/common
DEFINES += BUILD_TELEPHONYUTILS 
    
QT -= gui
QT += testlib

symbian: {
  CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  LIBS += -lmocklib -lsymbianmock
}

# test code
HEADERS += ut_cpphonelocalisation.h
SOURCES += ut_cpphonelocalisation.cpp

# code to be tested
HEADERS += ../../inc/cpphonelocalisation.h 
           
SOURCES += ../../src/cpphonelocalisation.cpp

# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_qtranslator.cpp 


symbian:MMP_RULES += SMPSAFE
