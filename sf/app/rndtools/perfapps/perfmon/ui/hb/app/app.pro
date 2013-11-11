#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#

TEMPLATE = app
TARGET = PerfMon
DEPENDPATH += .
INCLUDEPATH += inc

load(hb.prf)
symbian:CONFIG -= symbian_i18n

HEADERS += inc/application.h \
    inc/mainwindow.h \
    inc/mainview.h \
    inc/datacontainer.h \
    inc/valuedatacontainer.h \
    inc/graphdatacontainer.h \
    inc/settingsview.h \
    inc/datapopup.h \
    inc/enginewrapper.h

SOURCES += src/main.cpp \
    src/application.cpp \
    src/mainwindow.cpp \
    src/mainview.cpp \
    src/valuedatacontainer.cpp \
    src/graphdatacontainer.cpp \
    src/settingsview.cpp \
    src/datapopup.cpp \
    src/enginewrapper.cpp

RESOURCES +=

symbian: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
    INCLUDEPATH += ../../../engine/inc
    HEADERS += ../../../engine/inc/perfmon_engine.h \
            ../../../engine/inc/perfmon_powerlistener.h

    SOURCES += ../../../engine/src/perfmon_engine.cpp \
            ../../../engine/src/perfmon_powerlistener.cpp

    LIBS += -lestor \
        -lbafl \
        -lefsrv \
        -lavkon \
        -leikcore \
        -lapparc \
        -lapgrfx \
        -lgdi \
        -lcone \
        -lcentralrepository \
        -lHWRMPowerClient

    TARGET.CAPABILITY = WriteDeviceData

    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x20011385
    TARGET.SID = 0x20011385
    TARGET.VID = 0x101FB657 // Nokia

    TARGET.EPOCHEAPSIZE = 0x10000 0x2000000  // Min 64Kb, Max 32Mb

    ICON = ../../../icons/qgn_menu_perfmon.svg

    RSS_RULES += "group_name = \"RnD Tools\"";
}
