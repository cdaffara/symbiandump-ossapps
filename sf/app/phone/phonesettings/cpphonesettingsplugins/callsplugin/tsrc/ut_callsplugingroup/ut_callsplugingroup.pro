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

CONFIG += qtestlib hb
TEMPLATE = app
TARGET = 
DEPENDPATH += . ../../src/
INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../../cptelephonyutils/inc
INCLUDEPATH += /sf/mw/phonesrv/cellular/telephonysettings/xqbindings/psetwrapper/src
INCLUDEPATH += ../../../tsrc/common

DEFINES += BUILD_CALLSPLUGIN BUILD_TELEPHONYUTILS \
           BUILD_PSETWRAPPER BUILD_SSSETTINGSWRAPPER BUILD_PSUINOTES

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
HEADERS += ut_callsplugingroup.h
SOURCES += ut_callsplugingroup.cpp 
    
# code to be tested
HEADERS += ../../src/cpcallsplugin.h \
           ../../src/cpcallsplugingroup.h \
           ../../../cptelephonyutils/inc/cpplugincommon.h \
           /epoc32/include/platform/mw/psetwrapper.h \
           /epoc32/include/platform/mw/psuinotes.h \
           /epoc32/include/platform/mw/psetcliwrapper.h \
           /epoc32/include/platform/mw/sssettingswrapper.h \
           /epoc32/include/platform/mw/psetcallwaitingwrapper.h \
           /epoc32/include/platform/mw/psetcalldivertingwrapper.h \
           /epoc32/include/platform/mw/psetnetworkwrapper.h             

    
SOURCES += ../../src/cpcallsplugingroup.cpp
    
# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_cpcallsplugin.cpp \
           ../../../tsrc/mocks/mock_psuinotes.cpp \
           ../../../tsrc/mocks/mock_cpplugincommon.cpp \
           ../../../tsrc/mocks/mock_psetcalldivertingwrapper.cpp \
           ../../../tsrc/mocks/mock_psetcallwaitingwrapper.cpp \
           ../../../tsrc/mocks/mock_psetcliwrapper.cpp \
           ../../../tsrc/mocks/mock_psetnetworkwrapper.cpp \
           ../../../tsrc/mocks/mock_psetwrapper.cpp \
           ../../../tsrc/mocks/mock_sssettingswrapper.cpp

symbian:MMP_RULES += SMPSAFE
