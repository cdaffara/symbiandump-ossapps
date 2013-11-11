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
DEFINES += BUILD_NETWORKPLUGIN BUILD_PSETWRAPPER \
           BUILD_TELEPHONYUTILS BUILD_PSUINOTES

QT -= gui
QT += testlib

symbian: { 
    #CONFIG += no_icon
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    LIBS += -lmocklib \
        -lsymbianmock \
        -lcpframework 
        }
    
# test code
HEADERS += ut_networkpluginform.h
SOURCES += ut_networkpluginform.cpp
    
# code to be tested
HEADERS += ../../src/cpnetworkplugin.h \
           ../../src/cpnetworkpluginform.h \
           ../../src/cellulardatasettings.h \
           ../../../cptelephonyutils/inc/cpplugincommon.h \
           ../../../cptelephonyutils/inc/cpphonelocalisation.h \
           /epoc32/include/platform/mw/psetwrapper.h \
           /epoc32/include/platform/mw/psuinotes.h \
           /epoc32/include/platform/mw/psetcliwrapper.h \
           /epoc32/include/platform/mw/psetcallwaitingwrapper.h \
           /epoc32/include/platform/mw/psetcalldivertingwrapper.h \
           /epoc32/include/platform/mw/psetnetworkwrapper.h

# mocks needed for testing
SOURCES += ../../../tsrc/mocks/mock_cpnetworkplugin.cpp \
           ../../../tsrc/mocks/mock_psetwrapper.cpp \
           ../../../tsrc/mocks/mock_psetcalldivertingwrapper.cpp \
           ../../../tsrc/mocks/mock_psetcallwaitingwrapper.cpp \
           ../../../tsrc/mocks/mock_psetcliwrapper.cpp \
           ../../../tsrc/mocks/mock_psetnetworkwrapper.cpp \
           ../../../tsrc/mocks/mock_cpplugincommon.cpp \
           ../../../tsrc/mocks/mock_psuinotes.cpp \
           ../../../tsrc/mocks/mock_cpphonelocalisation.cpp \
           ../../../tsrc/mocks/mock_hbdialog.cpp \
           ../../../tsrc/mocks/mock_hbwidget.cpp \
           ../../../tsrc/mocks/mock_cellulardatasettings.cpp
           
SOURCES += ../../src/cpnetworkpluginform.cpp 
           

symbian:MMP_RULES += SMPSAFE
