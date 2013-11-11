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
#

CONFIG += mobility
load(hb.prf)
symbian:CONFIG -= symbian_i18n   // to prevent compile warnings from non-existent ts files
MOBILITY += systeminfo
TEMPLATE = app
TARGET = networkhandlingstarter

INCLUDEPATH += ./inc/
DEFINES += BUILD_NETWORKHANDLINGSTARTER
MOC_DIR = moc

symbian: {
    TARGET.CAPABILITY = CAP_APPLICATION
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002E67A
    VERSION = 10.0
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    LIBS += -lnetworkhandling
    LIBS += -lxqservice
    LIBS += -lxqserviceutil
    LIBS += -lcpframework
    LIBS += -lphonesettings
    LIBS += -ltstaskmonitorclient
    LIBS += -lcentralrepository

    BLD_INF_RULES.prj_exports +=  \
     "./rom/networkhandlingstarter.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(networkhandlingstarter.iby)"
    BLD_INF_RULES.prj_exports +=  \
    "./rom/networkhandlingstarter_stub.sis    /epoc32/data/z/system/install/networkhandlingstarter_stub.sis"
    RSS_RULES += "hidden = KAppIsHidden;"
}

HEADERS += ./inc/networkhandlingstarter.h
HEADERS += ./inc/networkhandlingstarter_p.h
HEADERS += ./inc/cnetworklistener.h

SOURCES += ./src/main.cpp
SOURCES += ./src/networkhandlingstarter.cpp
SOURCES += ./src/networkhandlingstarter_p.cpp
SOURCES += ./src/cnetworklistener.cpp

CONFIG += service
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable


symbian:MMP_RULES += SMPSAFE
