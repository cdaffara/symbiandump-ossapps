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
# Version : %version: 17 %


TEMPLATE = app
TARGET = testvideoplayerengine
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

symbian: 
{ 
    MMP_RULES += "USERINCLUDE stub/inc"
}


DEPENDPATH += inc src stub/src stub/inc

INCLUDEPATH +=stub/inc \
              ../../../../inc

CONFIG += qtestlib hb qt

LIBS += -lxqserviceutil.dll


# Input
HEADERS += videoplaybackwrapper.h \
           xqserviceutil.h \
           mpxviewpluginqt.h \
           testviewplugin.h \
           xqpluginloader.h \
           xqplugininfo.h \
           hbinstance.h \
           testvideoplayerengine.h \
           videoservices.h \
           videoserviceurifetch.h \
           videoserviceplay.h \
           videoserviceview.h \
           hbview.h \
           videoactivitystate.h \
           afactivitystorage.h \
	   videoiadupdatewrapper.h \
           ../../../../inc/videoplayerengine.h

SOURCES += videoplaybackwrapper.cpp \
           xqserviceutil.cpp \
           testviewplugin.cpp \
           xqpluginloader.cpp \
           hbinstance.cpp \
           testvideoplayerengine.cpp \
           videoservices.cpp \
           videoserviceurifetch.cpp \
           videoserviceplay.cpp \
           videoserviceview.cpp \
           videoactivitystate.cpp \
           afactivitystorage.cpp \
	   videoiadupdatewrapper.cpp \
           ../../src/videoplayerengine.cpp
