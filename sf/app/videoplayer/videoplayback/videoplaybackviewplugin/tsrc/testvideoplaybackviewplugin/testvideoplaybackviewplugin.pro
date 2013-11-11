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
# Version : %version: 6 %


TEMPLATE = app
TARGET = 
CONFIG += qtestlib qt hb

INCLUDEPATH += hbstub
INCLUDEPATH += inc stub/inc

DEPENDPATH += inc src stub/inc stub/src

LIBS += -lestor.dll \
        -lmpxviewframeworkqt.dll \
        -lxqplugins.dll

# Input
HEADERS += testvideoplaybackviewplugin.h \           
           videoplaybackview.h \
           ../../inc/videoplaybackviewplugin.h \
           ../../../../inc/mpxhbvideocommondefs.h 
               
SOURCES += testvideoplaybackviewplugin.cpp \
           videoplaybackview.cpp \					 
           ../../src/videoplaybackviewplugin.cpp 
