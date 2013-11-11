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
DEPENDPATH += . inc src 
              
INCLUDEPATH += . \
               ../../../inc \ 
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

LIBS += -lmpxcommon.dll -lflogger.dll -lestor.dll \
        -lfbscli.dll -lbitgdi.dll -lgdi.dll 

# Input
HEADERS += stub/inc/videocollectionclient.h \         
           inc/testvideodeleteworker.h \
           ../../inc/videodeleteworker.h 
          

SOURCES += src/testvideodeleteworker.cpp  \
           ../../src/videodeleteworker.cpp 
                     
