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
    src

INCLUDEPATH = stub/inc \
	/epoc32/include \
	/epoc32/include/osextensions/stdapis \
	/epoc32/include/osextensions/stdapis/sys \
	/epoc32/include/stdapis \
	/epoc32/include/stdapis/sys \
	. \
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \
    ../../../inc \
    
CONFIG += qtestlib \
    Hb \
    symbian_test
          
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll

RESOURCES += data/testvideothumbnaildata.qrc

# Input
HEADERS += inc/testvideothumbnaildata.h \
           stub/inc/videothumbnaildata_p.h \
           ../../inc/videothumbnaildata.h \
           ../../../inc/videocollectionexport.h
               
SOURCES += src/testvideothumbnaildata.cpp \
           stub/src/videothumbnaildata_p.cpp \
           ../../src/videothumbnaildata.cpp
