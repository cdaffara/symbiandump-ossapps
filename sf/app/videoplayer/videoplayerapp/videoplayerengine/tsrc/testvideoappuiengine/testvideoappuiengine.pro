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
# Version : %version: 5 %


TEMPLATE = app
TARGET = testvideoappuiengine
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

symbian: 
{ 
    MMP_RULES += "USERINCLUDE stub/inc"
}

INCLUDEPATH += stub/inc \
               ../../../../inc \
               $$MW_LAYER_SYSTEMINCLUDE \
               $$APP_LAYER_SYSTEMINCLUDE
              
DEPENDPATH += stub/inc stub/src inc src

CONFIG += qtestlib hb qt

LIBS += -lmpxcollectionhelper.dll \    
        -lmpxcommon.dll \
        -lefsrv.dll \
        -lcharconv.dll

# Input
HEADERS += videoplaybackwrapper.h \
           testvideoappuiengine.h \
           mpxcollectionutilityimp_stub.h \
           mpxplaybackutilityimp_stub.h \
           hbinstance.h \
           hbview.h \ 
           testutilities.h \
           mpxcollectionplaylist.h \
           videoplaylistutility.h \
           ../../inc/mpxvideoplayerappuiengine.h

SOURCES += videoplaybackwrapper.cpp \
           coneutils_stub.cpp \
           mpxcollectionutility_stub.cpp \
           mpxcollectionutilityimp_stub.cpp \
           testvideoappuiengine.cpp \
           mpxplaybackutilityimp_stub.cpp \
           hbinstance.cpp \
           testutilities.cpp \
           mediarecognizer_stub.cpp \
           mpxcollectionplaylist.cpp \
           videoplaylistutility.cpp \
           ../../src/mpxvideoplayerappuiengine.cpp 
