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
TARGET = cpcallsplugin
DEPENDPATH += .
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
SOURCES += src/cpcallsplugingroup.cpp \
    src/cpcallsplugin.cpp
			
HEADERS += src/cpcallsplugingroup.h \
    src/cpcallsplugin.h

include(../cpplugincommon.pri)

symbian: {
    TARGET.UID3 = 0X20029F26
    VERSION = 10.0
    LIBS += -lxqsysinfo \
        -lxqsettingsmanager
}

BLD_INF_RULES.prj_exports += "./rom/cpcallsplugin_stub.sis /epoc32/data/z/system/install/cpcallsplugin_stub.sis"

