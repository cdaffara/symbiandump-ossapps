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

TEMPLATE = app

CONFIG += debug_and_release

QT += testlib
QT += sql 

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
	} else {
    DESTDIR = ./release
	}

CONFIG += hb

HB += hbfeedback

DEFINES += HOMESCREEN_TEST \
           HSDOMAINMODEL_TEST


coverage: {
    DEFINES += COVERAGE_MEASUREMENT
}

INCLUDEPATH += ./inc \
               ../../inc \
               ../../../../inc \
               ../../../../hsdomainmodel/inc

DEPENDPATH += ./inc \
              ./src \
              ../../inc \
              ../../src

win32:include(t_hsclockwidgetplugin_win.pri)
symbian:include(t_hsclockwidgetplugin_symbian.pri)

symbian: {
    CONFIG += symbian_test
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    TARGET.UID3 = 0x20022F6D
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles WriteDeviceData ReadDeviceData
    LIBS += -lbafl -lapgrfx -lcone
    
    MMP_RULES += SMPSAFE

}

RESOURCES = ../../hsclockwidgetplugin.qrc

win32:include(installs_win32.pri)

