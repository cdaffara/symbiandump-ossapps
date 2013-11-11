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
# Version : %version: ou1cpsw#12 %

TEMPLATE = app
TARGET = testvideoplaybackview
CONFIG += qtestlib hb qt
DEFINES += BUILD_VIDEOPLAYBACK_DLL

DEPENDPATH += . inc src stub/src stub/inc

INCLUDEPATH += stub/inc \
               ../../inc \
               ../../../../inc \                          
               ../../controlinc \

LIBS += -lcone.dll \
        -lcommonengine.dll \
        -lflogger.dll
        
# Input
HEADERS += mpxvideoviewwrapper.h \					 
           testvideoplaybackview.h \
           ../../../inc/videobaseplaybackview.h \
           ../../../inc/videoplaybackview.h \
           videoplaybackviewfiledetails.h \
           hbdialog.h \
           hbmessagebox.h \
           hbnotificationdialog.h \
           hbtapgesture.h \
           hbpangesture.h \
           videoactivitystate.h

SOURCES += mpxvideoviewwrapper.cpp \					 				 
           testvideoplaybackview.cpp \
           ../../viewsrc/videobaseplaybackview.cpp \
           ../../viewsrc/videoplaybackview.cpp \
           ../../viewsrc/videoplaybackviewfiledetails.cpp \
           hbmessagebox.cpp \
           hbnotificationdialog.cpp \
           hbtapgesture.cpp \
           hbpangesture.cpp \
           videoactivitystate.cpp
