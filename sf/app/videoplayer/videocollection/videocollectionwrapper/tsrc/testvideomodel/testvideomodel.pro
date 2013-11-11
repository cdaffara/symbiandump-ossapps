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
DEPENDPATH += . inc src /stub/inc /stub/src
              
INCLUDEPATH += . \
               ../../../inc \ 
               stub/inc \
               /epoc32/include/domain \
               /epoc32/include/domain/middleware \
               /epoc32/include/domain/applications \
               /epoc32/include/osextensions \
               /epoc32/include/middleware \
               /epoc32/include/osextensions/stdapis/stlport \
               /VideoApp_Domain/video/videofeeds/inc

               
CONFIG += qtestlib \
          Hb \
          symbian_test

LIBS += -lmpxcommon.dll -lestor.dll \
       -lthumbnailmanagerqt.dll -lfbscli.dll -lbitgdi.dll -lgdi.dll 

# Input
HEADERS += ../../inc/videolistdatamodel.h \
           ../../inc/videodatasignalreceiver.h \
           ../../../inc/videocollectioncommon.h \           
           inc/testvideolistdatamodel.h \ 
           inc/mediaobjectfactory.h \
           stub/inc/videocollectionutils.h \
           stub/inc/videolistdatamodel_p.h \
           stub/inc/videocollectionclient.h \
           stub/inc/videodeleteworker.h \
           stub/inc/videocollectionwrapper.h 
         


SOURCES += ../../src/videolistdatamodel.cpp \           
           src/testvideolistdatamodel.cpp \
           src/mediaobjectfactory.cpp \
           stub/src/videocollectionutils.cpp \
           stub/src/videolistdatamodel_p.cpp \
           stub/src/videocollectionclient.cpp \
           stub/src/videodeleteworker.cpp \
           stub/src/videocollectionwrapper.cpp 
         
           
RESOURCES += data/testvideolistdatamodel.qrc           
