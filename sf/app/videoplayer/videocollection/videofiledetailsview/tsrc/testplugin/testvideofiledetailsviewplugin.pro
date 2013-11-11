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

TARGET = testvideofiledetailsviewplugin
TEMPLATE = app

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
    ../../../../inc \
    ../../../inc \
    ../../../videocollectionwrapper/inc


CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lestor.dll \
        -lfbscli.dll \
        -lbitgdi.dll \
        -lgdi.dll \
        -lxqplugins.dll \
        -lmpxviewframeworkqt.dll \
	-lxqserviceutil.dll \

# Input
HEADERS += inc/testvideofiledetails.h \
           stub/inc/videocollectionwrapper.h \
           stub/inc/videoproxymodelgeneric.h \
           stub/inc/hbmessagebox.h \
           stub/inc/shareui.h \
           stub/inc/thumbnailmanager_qt.h \
           stub/inc/videoservices.h \
           stub/inc/videoserviceurifetch.h \
           ../../inc/videofiledetailsviewplugin.h \
           ../../inc/videodetailslabel.h
   
SOURCES += src/testvideofiledetails.cpp \
           stub/src/videocollectionwrapper.cpp \
           stub/src/videoproxymodelgeneric.cpp \
           stub/src/hbmessagebox.cpp \
           stub/src/shareui.cpp \
           stub/src/thumbnailmanager_qt.cpp \
           stub/src/videoservices.cpp \
           stub/src/videoserviceurifetch.cpp \
           ../../src/videofiledetailsviewplugin.cpp \
           ../../src/videodetailslabel.cpp

RESOURCES += ../../data/videofiledetails.qrc
TARGET.CAPABILITY = All \
    -Tcb
