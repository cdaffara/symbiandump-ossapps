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
TARGET = cpnetworkplugin
CONFIG += hb plugin mobility
MOBILITY += systeminfo

INCLUDEPATH += ../cptelephonyutils/inc 

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

TRANSLATIONS = telephone_cp.ts

LIBS += -lcpframework \
        -lpsetwrapper \
        -lcptelephonyutils \
        -lpsuinotes

symbian*::LIBS += -lcmmanager

include(../cpplugincommon.pri)
include (cpnetworkplugin.pri)

symbian: { 
    TARGET.UID3 = 0X20029F24
    VERSION = 10.0
}

BLD_INF_RULES.prj_exports += "./rom/cpnetworkplugin_stub.sis /epoc32/data/z/system/install/cpnetworkplugin_stub.sis"
