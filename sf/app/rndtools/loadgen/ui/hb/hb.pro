# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
TEMPLATE = app
TARGET = LoadGen
DEPENDPATH += .
INCLUDEPATH += . \
    /inc
INCLUDEPATH += ./inc \
    ../../engine/inc
load(hb.prf)
symbian:CONFIG -= symbian_i18n
HEADERS += inc/mainview.h \
    inc/settingsview.h \
    inc/notifications.h
SOURCES += src/main.cpp \
    src/mainview.cpp \
    src/settingsview.cpp \
    src/notifications.cpp
RESOURCES += loadgen.qrc
symbian: { 
    MMP_RULES -= PAGED
    MMP_RULES *= UNPAGED
    BLD_INF_RULES.prj_exports += "./rom/loadgen.iby CORE_IBY_EXPORT_PATH(tools,loadgen.iby)"
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += /epoc32/include/mw/http
    HEADERS += inc/engine.h \
        inc/enginewrapper.h
    SOURCES += src/engine.cpp \
        src/enginewrapper.cpp \
        ../../engine/src/loadgen_utils.cpp \
        ../../engine/src/loadgen_loadbase.cpp \
        ../../engine/src/loadgen_cpuload.cpp \
        ../../engine/src/loadgen_memoryeat.cpp \
        ../../engine/src/loadgen_phonecall.cpp \
        ../../engine/src/loadgen_keypress.cpp \
        ../../engine/src/loadgen_messages.cpp \
        ../../engine/src/loadgen_applications.cpp \
        ../../engine/src/loadgen_photocapture.cpp \
        ../../engine/src/loadgen_bluetooth.cpp \
        ../../engine/src/loadgen_pointerevent.cpp \
        ../../engine/src/loadgen_netconn.cpp \
        ../../engine/src/loadgen_httpreceiver.cpp
    RSS_RULES += "group_name = \"RnD Tools\"";
    LIBS += -leuser \
        -lcommonengine \
        -lapparc \
        -lcone \
        -leikcore \
        -leikcoctl \
        -leikctl \
        -leikdlg \
        -lws32 \
        -lapgrfx \
        -lefsrv \
        -lbafl \
        -lgdi \
        -legul \
        -letel3rdparty \
        -lcommdb \
        -lflogger \
        -lestor \
        -lmsgs \
        -lsmcm \
        -lgsmu \
        -letext \
        -lsendas2 \
        -lecam \
        -lfeatdiscovery \
        -lcentralrepository \
        -lesock \
        -lbluetooth \
        -lhal \
        -lhttp \
        -linetprotutil
    TARGET.CAPABILITY = swevent \
        ReadUserData \
        WriteUserData \
        NetworkServices \
        UserEnvironment \
        CAP_APPLICATION \
        AllFiles
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x20011384
    // TARGET.SID = 0x20011384
    TARGET.VID = 0x101FB657 \
        // \
        Nokia
    TARGET.EPOCHEAPSIZE = 0x10000 \
        0x1000000 \
        // \
        Min \
        64Kb, \
        Max \
        16Mb
    ICON = ../../icons/qgn_menu_loadgen.svg
}
else:error("Only Symbian supported!")
