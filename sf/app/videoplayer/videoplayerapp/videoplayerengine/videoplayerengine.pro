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
# Description: Project file for building Videoplayer components
#
#
# Version : %version: 34 %


TEMPLATE = lib
CONFIG += hb qt dll
TARGET = videoplayerengine
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

symbian: 
{ 
    TARGET.CAPABILITY = ALL -DRM -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20024337
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
                   $$APP_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/videoplayerengine.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videoplayerengine.iby)"
    defBlock = \      
        "$${LITERAL_HASH}if defined(EABI)" \
        "DEFFILE ../eabi/videoplayerengine.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE ../bwins/videoplayerengine.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defBlock SMPSAFE
    VERSION = 10.0
}

INCLUDEPATH += ../../inc \
               ../inc

LIBS += -lmpxviewframeworkqt.dll \
        -lxqplugins.dll \
        -lmpxplaybackutility.dll \
        -lmpxcollectionutility.dll \
        -lmpxcollectionhelper.dll \    
        -lmpxcommon.dll \
        -lplaybackhelper.dll \
        -lvideoplaylistutility.dll \
        -lservicehandler.dll \
        -lxqservice.dll \
        -lxqserviceutil.dll \
        -lflogger.dll \
        -lefsrv \
        -lcommonengine.dll \
        -lafservice.dll \
        -liaupdateapi.dll \
        -lfeatmgr.dll

DEPENDPATH += ../../inc ../inc inc
VPATH += src

HEADERS += videoplayerengine.h \
           videoplaybackwrapper.h \
           mpxvideoplayerappuiengine.h \
           videoservices.h \
           videoserviceplay.h \
           videoserviceurifetch.h \
           videoserviceview.h \
           videoservicebrowse.h \
           videoactivitystate.h \
           videoserviceuri.h \
           videoiadupdatewrapper.h 

SOURCES += videoplayerengine.cpp \
           videoplaybackwrapper.cpp \
           mpxvideoplayerappuiengine.cpp \
           videoservices.cpp \
           videoserviceplay.cpp \
           videoserviceurifetch.cpp \
           videoserviceview.cpp \
           videoservicebrowse.cpp \
           videoactivitystate.cpp \
           videoserviceuri.cpp \
           videoiadupdatewrapper.cpp
