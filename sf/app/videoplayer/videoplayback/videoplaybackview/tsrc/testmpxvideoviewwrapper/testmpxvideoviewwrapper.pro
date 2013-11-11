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
# Description: Project file for building testmpxvideoviewwrapper
#
#
# Version : %version: 11 %


TEMPLATE = app
TARGET = testmpxvideoviewwrapper
CONFIG += qtestlib hb qt

DEPENDPATH += inc src stub/src stub/inc

INCLUDEPATH = hbstub 
INCLUDEPATH += stub/inc \
               ../inc \
               ../../inc \
               ../../../inc \
               ../../../../inc                                          

LIBS += -lmpxcommon.dll \
        -lflogger.dll \
        -lcone.dll \
        -lws32.dll

# Input
HEADERS += testmpxvideoviewwrapper.h \
           stub/inc/videobaseplaybackview.h  \
           videoplaybackuserinputhandler.h \
           videoplaybackcontrolscontroller.h \           
           videoplaybackviewfiledetails.h \
           mpxplaybackutility.h \
           mpxcollectionutility.h \
           mpxcollectionplaylist.h \
           devsoundif.h \
           ../../viewinc/mpxvideoviewwrapper.h
					                
SOURCES += testmpxvideoviewwrapper.cpp \
           stub/src/videobaseplaybackview.cpp \
           videoplaybackdisplayhandler.cpp \					 
           videoplaybackviewfiledetails.cpp \
           videoplaybackcontrolscontroller.cpp \
           mpxplaybackutility.cpp \
           mpxcollectionutility.cpp \
           mpxcollectionplaylist.cpp \
           videoplaybackuserinputhandler.cpp \
           devsoundif.cpp \
           ../../viewsrc/mpxvideoviewwrapper.cpp
