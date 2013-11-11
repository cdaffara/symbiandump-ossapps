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
    \ # keep these at bottom so that stubbed headers are taken first
    ../../../inc \
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc \
    ../../../../inc \

HEADERS += inc/testlisttoolbar.h \
    \ # headers needed in test
    ../../../videocollectionview/inc/videolisttoolbar.h \

SOURCES += src/testlisttoolbar.cpp \
    \ # sources needed in test
    ../../../videocollectionview/src/videolisttoolbar.cpp \

TESTEDCLASS = videolisttoolbar
include(../../../tsrc/stubs/stubs.pro)

RESOURCES += ../../data/videocollectionview.qrc           
