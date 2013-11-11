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
DEFINES     += BUILD_VIDEOCOLLECTION_DLL

DEPENDPATH += . \
    inc \
    src \
    stub/inc \
    stub/src
    
INCLUDEPATH += . \
    stub/inc \
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \
    ../../../inc

CONFIG += qtestlib \
          Hb \
          symbian_test
          
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll

# Input
HEADERS +=  inc/testvideoproxymodelcontent.h \
            stub/inc/filterproxytester.h \
            stub/inc/videoproxymodelgeneric.h \
            stub/inc/videolistdatamodel.h \
            ../../inc/videoproxymodelcontent.h
    
SOURCES += src/testvideoproxymodelcontent.cpp \
           stub/src/filterproxytester.cpp \
           stub/src/videoproxymodelgeneric.cpp \
           stub/src/videolistdatamodel.cpp \
           ../../src/videoproxymodelcontent.cpp
