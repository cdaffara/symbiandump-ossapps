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
              
INCLUDEPATH += . \
                inc \
                stub/inc \
               ../../../inc \
               /epoc32/include/domain \
               /epoc32/include/domain/middleware \
               /epoc32/include/domain/applications \
               /epoc32/include/osextensions \
               /epoc32/include/middleware \
              /epoc32/include/osextensions/stdapis/stlport

LIBS += -lflogger.dll \
        -lestor.dll \
        -lfbscli.dll \
        -lbitgdi.dll \
        -lgdi.dll \
        -lmpxcommon.dll

# Input
HEADERS += inc/testvideocollectionlistener.h \ 
           inc/mediaobjectfactory.h \
           stub/inc/stubcollection.h  \ 
           stub/inc/videocollectionclient.h  \
           stub/inc/stubsignalreceiver.h \
           ../../inc/videocollectionutils.h \
           ../../inc/videodatasignalreceiver.h \
           ../../inc/videocollectionlistener.h

SOURCES += src/testvideocollectionlistener.cpp \
           src/mediaobjectfactory.cpp  \
           stub/src/stubcollection.cpp \
           stub/src/videocollectionclient.cpp  \
           stub/src/stubsignalreceiver.cpp \
           ../../src/videocollectionutils.cpp \
           ../../src/videocollectionlistener.cpp
