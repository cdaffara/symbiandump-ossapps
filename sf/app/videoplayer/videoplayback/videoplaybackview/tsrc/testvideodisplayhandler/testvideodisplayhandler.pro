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
# Version : %version: 7 %

TEMPLATE = app
TARGET = testvideodisplayhandler
CONFIG += qtestlib hb qt
DEFINES += BUILD_VIDEOPLAYBACK_DLL

DEPENDPATH += inc src stub/inc stub/src

INCLUDEPATH = hbstub
INCLUDEPATH += stub/inc \
               ../../../inc \
               ../../../../inc

LIBS += -lmpxcommon.dll \
        -lestor.dll \
        -lsysutil.dll \
        -lcone.dll \
        -lefsrv.dll \
        -lws32.dll \
        -lgdi.dll
                
# Input
HEADERS += mpxvideoviewwrapper.h \
           videobaseplaybackview.h \
           videoplaybackviewfiledetails.h \
           videocontainer.h \
           alfcompositionutility.h \
           mediaclientvideodisplay.h \
           testvideodisplayhandler.h \
           ../../viewinc/videoplaybackdisplayhandler.h
                                                
SOURCES += mpxvideoviewwrapper.cpp \					 			
           videobaseplaybackview.cpp \
           videoplaybackviewfiledetails.cpp \
           videocontainer.cpp \
           alfcompositionutility.cpp \
           mediaclientvideodisplay.cpp \
           testvideodisplayhandler.cpp \
           ../../viewsrc/videoplaybackdisplayhandler.cpp
