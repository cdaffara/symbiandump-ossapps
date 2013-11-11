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
    -lvideocollectionwrapper.dll \
    -lvideoplayerengine.dll

HEADERS += inc/testvideocollectionuiloader.h \
    ../../../videocollectionview/inc/videocollectionuiloader.h \
    ../../../videocollectionview/inc/videocollectionuiloaderdef.h 

SOURCES += src/testvideocollectionuiloader.cpp \ 
    \ # sources needed in test
    ../../../videocollectionview/src/videocollectionuiloader.cpp
    
TESTEDCLASS = videocollectionuiloader
include(../../../tsrc/stubs/stubs.pro)

RESOURCES += ../../data/videocollectionview.qrc
