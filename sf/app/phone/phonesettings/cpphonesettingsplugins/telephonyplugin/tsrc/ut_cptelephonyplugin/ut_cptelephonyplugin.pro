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
DEFINES += 

QT -= gui
QT += testlib

symbian: {
  #CONFIG += no_icon
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  
  LIBS += -lmocklib \
        -lsymbianmock \
        -lcpframework \
        -lcptelephonyutils
}

# test code
HEADERS += ut_cptelephonyplugin.h
SOURCES += ut_cptelephonyplugin.cpp

# code to be tested
HEADERS += ../../src/cptelephonyplugin.h \
           ../../src/cptelephonypluginview.h
           
SOURCES += ../../src/cptelephonyplugin.cpp 

# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_cptelephonypluginview.cpp


symbian:MMP_RULES += SMPSAFE
