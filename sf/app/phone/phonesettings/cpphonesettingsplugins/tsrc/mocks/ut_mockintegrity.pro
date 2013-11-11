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
MOC_DIR = moc

INCLUDEPATH += .
INCLUDEPATH += ../../barringplugin/inc
INCLUDEPATH += ../../callsplugin/src
INCLUDEPATH += ../../cptelephonyutils/inc
INCLUDEPATH += ../../divertplugin/src
INCLUDEPATH += ../../cpnetworkplugin/src
INCLUDEPATH += ../../cpnetworkplugin/src
INCLUDEPATH += ../../telephonyplugin/src
INCLUDEPATH += /sf/mw/phonesrv/cellular/telephonysettings/xqbindings/psetwrapper/src

DEFINES += BUILD_CPTELEPHONYUTILS BUILD_PSETWRAPPER BUILD_SSSETTINGSWRAPPER BUILD_PSUINOTES
DEFINES += XQSYSINFO_LIBRARY XQSETTINGSMANAGER_LIBRARY QT_BUILD_SYSINFO_LIB QT_MAKEDLL

QT -= gui
QT += testlib

symbian: { 
    CONFIG += no_icon
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    LIBS += -lmocklib       \
            -lsymbianmock   \
            -lcpframework 
}

HEADERS += ut_mockintegrity.h

HEADERS += ../../callsplugin/src/*.h                            \
           ../../barringplugin/inc/*.h                          \
           ../../cpnetworkplugin/src/*.h                        \
           ../../cptelephonyutils/inc/cpplugincommon.h          \
           ../../cptelephonyutils/inc/cpphonelocalisation.h     \
           ../../telephonyplugin/src/cptelephonypluginview.h    \
           /epoc32/include/platform/mw/psetwrapper.h            \
           /epoc32/include/platform/mw/psuinotes.h              \
           /epoc32/include/platform/mw/psetcliwrapper.h         \
           /epoc32/include/platform/mw/sssettingswrapper.h      \
           /epoc32/include/platform/mw/psetcallwaitingwrapper.h \
           /epoc32/include/platform/mw/psetcalldivertingwrapper.h \
           /epoc32/include/platform/mw/psetnetworkwrapper.h     \
           /epoc32/include/platform/mw/psetcallbarringwrapper.h \
           /epoc32/include/platform/mw/xqsettingskey.h          \
           /epoc32/include/platform/mw/xqsettingsmanager.h      \
           /epoc32/include/platform/mw/xqsysinfo.h              \
           /epoc32/include/mw/qsysteminfo.h

SOURCES += *.cpp

  

symbian:MMP_RULES += SMPSAFE
