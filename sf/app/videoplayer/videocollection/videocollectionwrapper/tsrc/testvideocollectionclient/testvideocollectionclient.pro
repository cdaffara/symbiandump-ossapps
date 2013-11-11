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
              stub/src \
              stub/inc

CONFIG += qtestlib \
          Hb \
          symbian_test      
              
INCLUDEPATH =   inc \
                stub/inc \
                /epoc32/include \
                /epoc32/include/osextensions/stdapis \
                /epoc32/include/osextensions/stdapis/sys \
                /epoc32/include/stdapis \
                /epoc32/include/stdapis/sys \
                . \
                /epoc32/include \
                /epoc32/include/domain \
                /epoc32/include/domain/middleware \
                /epoc32/include/domain/applications \
                /epoc32/include/osextensions \
                /epoc32/include/middleware \
                /epoc32/include/osextensions/stdapis/stlport \
                ../../../inc

LIBS += -lflogger.dll \
        -lestor.dll \
        -lfbscli.dll \
        -lbitgdi.dll \
        -lgdi.dll

# Input
HEADERS += inc/testvideocollectionclient.h \
           stub/inc/mpxcollectionutility.h \
           stub/inc/mpxcollectionpath.h \
           stub/inc/mpxmedia.h \
           stub/inc/mpxmediaarray.h \
           stub/inc/mpxattribute.h \
           stub/inc/stubcollectionsignalreceiver.h \
           stub/inc/videocollectionlistener.h \
           ../../inc/videodatasignalreceiver.h \
           ../../inc/videocollectionclient.h

SOURCES += src/testvideocollectionclient.cpp \
           stub/src/mpxcollectionutility.cpp \
           stub/src/mpxmedia.cpp \
           stub/src/mpxcollectionpath.cpp \
           stub/src/videocollectionlistener.cpp \
           ../../src/videocollectionclient.cpp
