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
INCLUDEPATH += ../../../tsrc/common

DEFINES += BUILD_TELEPHONYUTILS BUILD_BARRINGPLUGIN \
           BUILD_PSETWRAPPER BUILD_SSSETTINGSWRAPPER BUILD_PSUINOTES
DEFINES += QT_NO_DEBUG_STREAM

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
HEADERS += ut_barringplugingroup.h
SOURCES += ut_barringplugingroup.cpp 
    
# code to be tested
HEADERS += ../../inc/cpbarringplugin.h \
           ../../inc/cpbarringplugingroup.h \
           ../../../cptelephonyutils/inc/cpplugincommon.h \
           /epoc32/include/platform/mw/psetwrapper.h \
           /epoc32/include/platform/mw/psuinotes.h \
           /epoc32/include/platform/mw/psetcallbarringwrapper.h \
           /epoc32/include/platform/mw/psetcalldivertingwrapper.h \
           /epoc32/include/platform/mw/psetcallwaitingwrapper.h \
           /epoc32/include/platform/mw/psetcliwrapper.h \
           /epoc32/include/platform/mw/psetnetworkwrapper.h \
           #/epoc32/include/platform/mw/sssettingswrapper.h \

SOURCES += ../../src/cpbarringplugingroup.cpp

# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_cpbarringplugin.cpp
SOURCES += ../../../tsrc/mocks/mock_psetcallbarringwrapper.cpp
SOURCES += ../../../tsrc/mocks/mock_psetcalldivertingwrapper.cpp
SOURCES += ../../../tsrc/mocks/mock_psetcallwaitingwrapper.cpp
SOURCES += ../../../tsrc/mocks/mock_psetcliwrapper.cpp
SOURCES += ../../../tsrc/mocks/mock_psetnetworkwrapper.cpp
SOURCES += ../../../tsrc/mocks/mock_psetwrapper.cpp
SOURCES += ../../../tsrc/mocks/mock_psuinotes.cpp
SOURCES += ../../../tsrc/mocks/mock_cpplugincommon.cpp
SOURCES += ../../../tsrc/mocks/mock_cpitemdatahelper.cpp
SOURCES += ../../../tsrc/mocks/mock_hbcheckbox.cpp
SOURCES += ../../../tsrc/mocks/mock_qtranslator.cpp


symbian:MMP_RULES += SMPSAFE
