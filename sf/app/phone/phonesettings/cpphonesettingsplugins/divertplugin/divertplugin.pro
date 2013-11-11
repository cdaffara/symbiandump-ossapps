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
TARGET = cpdivertplugin
DEPENDPATH  += .
INCLUDEPATH += .
INCLUDEPATH += ../cptelephonyutils/inc
CONFIG += hb plugin

TRANSLATIONS = telephone_cp.ts

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc
LIBS += -lcpframework \
        -lpsetwrapper \
        -lsssettingswrapper \
        -lcptelephonyutils \
        -lpsuinotes

# Input
SOURCES += src/cpdivertplugingroup.cpp \
    src/cpdivertplugin.cpp \
    src/cpdivertitemdata.cpp \
    src/cpdivertquery.cpp

HEADERS += src/cpdivertplugingroup.h \
    src/cpdivertplugin.h \
    src/cpdivertitemdata.h \
    src/cpdivertquery.h

include(../cpplugincommon.pri)

symbian: {
    TARGET.UID3 = 0X20029F25
    LIBS += -lxqsysinfo \
        -lxqsettingsmanager
    VERSION = 10.0
}

BLD_INF_RULES.prj_exports += "./rom/cpdivertplugin_stub.sis /epoc32/data/z/system/install/cpdivertplugin_stub.sis"
