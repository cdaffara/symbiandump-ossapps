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

TEMPLATE = app

TARGET =

DEPENDPATH += . \
    inc \
    src \
    ../../../tsrc/stubs

INCLUDEPATH += . \
    inc \
    ../../../tsrc/stubs/inc \
    \ # keep these at bottom so that stub headers are taken first
    ../../../inc \
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc \
    ../../../../inc \
    ../../../../videoplayerapp/inc \
    ../../../../videoplayerapp/videoplayerengine/inc
    
CONFIG += qtestlib \
    hb

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lcentralrepository.dll \
    -lxqservice.dll \
    -lxqserviceutil.dll \
    -lapparc \
    -lapgrfx \
    -lws32 \
    -lcone
        
HEADERS += inc/testvideooperatorservice.h \
    ../../../videocollectionview/inc/videooperatorservice.h \
    ../../../videocollectionview/inc/videooperatorservice_p.h

SOURCES += src/testvideooperatorservice.cpp \ 
    \ # sources needed in test
    ../../../videocollectionview/src/videooperatorservice.cpp \
    ../../../tsrc/stubs/src/videooperatorservice_p.cpp
    
TESTEDCLASS = videooperatorservice
include(../../../tsrc/stubs/stubs.pro)
