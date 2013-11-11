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
    
INCLUDEPATH = . \ 
    inc \
    ../../../tsrc/stubs/inc \
    \ # keep these at bottom so that stub headers are taken first
    ../../../inc \
    ../../../../inc \    
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc
    
CONFIG += qtestlib \
    hb

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll
    
HEADERS += inc/testvideolistselectiondialog.h \
	inc/videolistselectiondialogtester.h \
    \ # headers needed in test
    ../../inc/videolistselectiondialog.h \
   
    
SOURCES += src/testvideolistselectiondialog.cpp \           
    \ # sources needed in test
    ../../src/videolistselectiondialog.cpp \
   
TESTEDCLASS = videolistselectiondialog
include(../../../tsrc/stubs/stubs.pro)
