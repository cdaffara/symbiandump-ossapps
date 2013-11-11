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

TEMPLATE = lib
TARGET = cptelephonyplugin
DEPENDPATH += . 
INCLUDEPATH += .
INCLUDEPATH += ../cptelephonyutils/inc
CONFIG += hb plugin

TRANSLATIONS = telephone_cp.ts

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

LIBS += -lcpframework \
        -lcptelephonyutils


# Input
HEADERS += src/cptelephonyplugin.h \
           src/cptelephonypluginview.h
SOURCES += src/cptelephonyplugin.cpp \
           src/cptelephonypluginview.cpp

include(../cpplugincommon.pri)

symbian: {
    LIBS += -lxqsysinfo \
        -lxqsettingsmanager
        
    TARGET.UID3 = 0X20029F23
    VERSION = 10.0
}

BLD_INF_RULES.prj_exports += "./rom/cptelephonyplugin_stub.sis /epoc32/data/z/system/install/cptelephonyplugin_stub.sis"
