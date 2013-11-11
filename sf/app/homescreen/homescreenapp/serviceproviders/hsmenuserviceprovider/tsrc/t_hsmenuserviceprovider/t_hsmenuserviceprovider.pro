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

include (../../../../common.pri)

CONFIG += mobility
MOBILITY += publishsubscribe

LIBS += -lhsdomainmodel        
LIBS += -lhsutils
LIBS += -lcasoftwareregistry

CONFIG += debug_and_release \
    console
CONFIG += hb
QT += testlib \
    xml \
    sql

#DEFINES += CONTENT_ARSENAL_STORAGE_UT
DEFINES += MENUSERVICE_TEST
#DEFINES += CACLIENT_TEST
DEFINES += MENUSERVICE_UNITTEST
DEFINES += HSDOMAINMODEL_TEST
coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH += . \
    ./src \
    ./inc \
    ../src \
    ../inc
    
#add new paths and put existing at the end to pick up mock files first
INCLUDEPATH = . \
    ./inc \
    ../../inc \
    ../../../../../homescreenapp/inc \
    $$INCLUDEPATH
    
symbian: {
    ### get rid of localization warnings
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    ###
    
    CONFIG += symbian_test
    coverage: CONFIG -= symbian_test
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x20022F6A
	TARGET.EPOCALLOWDLLDATA=1
	TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCSTACKSIZE = 0x14000 \
        // \
        80kB
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x1000000 \
        // \
        128kB \
        - \
        16MB
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
}
include(t_hsmenuserviceprovider.pri)
win32:include(installs_win32.pri)
!symbian:unix:include(installs_unix.pri)


symbian:MMP_RULES += SMPSAFE
